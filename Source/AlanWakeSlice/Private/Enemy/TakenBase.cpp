// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/TakenBase.h"

#include "TimerManager.h"
#include "AI/TakenAIController.h"
#include "Components/DarknessShield.h"
#include "Engine/World.h"
#include "Game/AWGameMode.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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
		DarknessShieldComponent->OnDarknessShieldDepletedDelegate.AddDynamic(this, &ATakenBase::OnShieldDestroyed);
	}
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
			ActiveFlashlightVFX = UNiagaraFunctionLibrary::SpawnSystemAttached(
				FlashlightHitVFX, GetMesh(), NAME_None,
				HitLocation, HitNormal.Rotation(),
				EAttachLocation::KeepWorldPosition, true);
		}
		else
		{
			ActiveFlashlightVFX->SetWorldLocationAndRotation(HitLocation, HitNormal.Rotation());
		}

		GetWorldTimerManager().SetTimer(FlashlightVFXStopTimer, this, &ATakenBase::StopFlashlightVFX, 0.15f, false);
	}
	if (bIsFocusBeam)
	{
		GetCharacterMovement()->MaxWalkSpeed = FocusedWalkSpeed;
		GetWorldTimerManager().SetTimer(SpeedResetTimer, this, &ATakenBase::ResetSpeed, 0.25f,  false);
		bIsBeingFocused = true;
		if (!bHasFlinched)
		{
			bHasFlinched = true;
			BP_OnFocusFlinch();
		}
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
	//TODO Trigger VFX/AUDIO in BP
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
}

void ATakenBase::ResetSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	bHasFlinched = false;
	bIsBeingFocused = false;

	BP_StopShieldAudio();
}



