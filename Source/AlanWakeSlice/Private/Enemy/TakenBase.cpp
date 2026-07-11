// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/TakenBase.h"

#include "AudioMixer.h"
#include "TimerManager.h"
#include "AI/TakenAIController.h"
#include "Components/DarknessShield.h"
#include "Engine/World.h"
#include "Game/AWGameMode.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ATakenBase::ATakenBase()
{
	PrimaryActorTick.bCanEverTick = false;
	DarknessShieldComponent = CreateDefaultSubobject<UDarknessShield>(TEXT("DarknessShieldComponent"));
	AIControllerClass = ATakenAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	
}
void ATakenBase::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	CurrentHealth = MaxHealth;

	if (DarknessShieldComponent)
	{
		DarknessShieldComponent->OnShieldValueChangedDelegate.AddDynamic(this, &ATakenBase::OnShieldValueChanged);
		DarknessShieldComponent->OnDarknessShieldDepletedDelegate.AddDynamic(this, &ATakenBase::OnShieldDestroyed);
	}
}

void ATakenBase::OnShieldValueChanged(float ShieldPercent)
{

}

void ATakenBase::ReceiveFlashlightExposure(float ExposureValue, bool bIsFocusBeam, const FVector& HitLocation, const FVector& HitNormal) 
{
	if (DarknessShieldComponent)
	{
		DarknessShieldComponent->ProcessExposure(ExposureValue, bIsFocusBeam);
		if (!DarknessShieldComponent->bIsVulnerable)
		{
			float ShieldPct = DarknessShieldComponent->CurrentShield / DarknessShieldComponent->MaxShield;
			if (DarknessShieldComponent->bRequiresFocusBeam && !bIsFocusBeam) return;
			BP_UpdateShieldAudio(ShieldPct);
		}
	}
	if (FlashlightHitVFX && DarknessShieldComponent && !DarknessShieldComponent->bIsVulnerable)
	{
		if (!ActiveFlashlightVFX)
		{
			if (GetMesh() && GetMesh()->SkeletalMesh)
			{
				UE_LOG(LogTemp, Warning, TEXT("Currently checking mesh: %s"), *GetMesh()->SkeletalMesh->GetName());
			}
			FName TargetSocket = TEXT("ShieldBurstSocket");
			// Debug check to verify if the mesh actually knows this socket exists
			if (!GetMesh()->DoesSocketExist(TargetSocket))
			{
				UE_LOG(LogTemp, Error, TEXT("CRITICAL: Socket %s NOT FOUND on Mesh!"), *TargetSocket.ToString());
			}
			ActiveFlashlightVFX = UNiagaraFunctionLibrary::SpawnSystemAttached(
					FlashlightHitVFX,
					GetMesh(),
					TargetSocket,
					FVector::ZeroVector,
					FRotator::ZeroRotator,
					EAttachLocation::SnapToTarget,
					/*bAutoDestroy=*/ true
				);
		}

		GetWorldTimerManager().SetTimer(FlashlightVFXStopTimer, this, &ATakenBase::StopFlashlightVFX, 0.15f, false);
	}
	if (bIsFocusBeam && !DarknessShieldComponent->bIsVulnerable)
	{
		bIsBeingFocused = true;
		GetCharacterMovement()->MaxWalkSpeed = FocusedWalkSpeed;
		GetWorldTimerManager().SetTimer(SpeedResetTimer, this, &ATakenBase::ResetSpeed, 0.25f,  false);
		if (!bHasFlinched)
		{
			bHasFlinched = true;
			BP_OnFocusFlinch();
		}
		BP_OnFlashlightHit(HitLocation, ExposureValue, bIsFocusBeam);
	}
}

float ATakenBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	if (DarknessShieldComponent && !DarknessShieldComponent->bIsVulnerable)
	{
		return 0.0f; 
	}

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	CurrentHealth -= ActualDamage;

	if (CurrentHealth <= 0.0f)
	{
		BP_OnDeath();
	}

	return ActualDamage;
}

void ATakenBase::OnShieldDestroyed()
{
	BP_OnShieldDestroyed();
	StopFlashlightVFX();
	if (ShieldBreakVFX)
	{
		const FVector BurstLocation = GetActorLocation() +
			FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.5f);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), ShieldBreakVFX, BurstLocation, GetActorRotation(),
			FVector(1.f), true, true);
	}
	BP_StopShieldAudio();
}

void ATakenBase::StopFlashlightVFX()
{
	if (ActiveFlashlightVFX)
	{
		ActiveFlashlightVFX->Deactivate();
		ActiveFlashlightVFX = nullptr;
	}
	BP_OnFlashlightStopped();
}

void ATakenBase::ResetSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	bHasFlinched = false;
	bIsBeingFocused = false;

	BP_StopShieldAudio();
}



