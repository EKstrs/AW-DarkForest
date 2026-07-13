// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/TakenBase.h"

#include "AudioMixer.h"
#include "TimerManager.h"
#include "AI/TakenAIController.h"
#include "Components/DarknessShield.h"
#include "Engine/World.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/SkeletalMesh.h"
#include "Game/AWGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"

ATakenBase::ATakenBase()
{
	PrimaryActorTick.bCanEverTick = true;
	DarknessShieldComponent = CreateDefaultSubobject<UDarknessShield>(TEXT("DarknessShieldComponent"));
	AIControllerClass = ATakenAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	MeleeWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeleeWeaponMesh"));
	MeleeWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeleeWeaponMesh->SetupAttachment(GetMesh(), TEXT("MeleeSocket"));
	
	
	
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
	if (AAWGameMode* GM =  Cast<AAWGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->RegisterActiveEnemy(this);
	}
}

void ATakenBase::EndPlay(const EEndPlayReason::Type reason)
{

	if (GetWorld())
	{
		if (AAWGameMode* GM =  Cast<AAWGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->UnregisterEnemy(this);
		}
	}
	Super::EndPlay(reason);
}

void ATakenBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsFlinching)
	{
		ReactionAlpha = FMath::FInterpConstantTo(ReactionAlpha, 1.0f, DeltaTime, 15.f);
		if (ReactionAlpha >= 1.0f)
		{
			bIsFlinching = false;
		}
	}
	
	else if (ReactionAlpha > 0.0f)
	{
		ReactionAlpha = FMath::FInterpTo(ReactionAlpha, 0.0f, DeltaTime, 3.f);
	}
}

void ATakenBase::PerformMeleeAttack()
{
	if (!bIsAttacking)
	{
		bIsAttacking = true;
		BP_Attack();
	}
	
}

USceneComponent* ATakenBase::GetTraceData(FName& OutStartSocket, FName& OutEndSocket)
{
	USceneComponent* TraceComponent = GetMesh();
	OutStartSocket = FName(TEXT("TraceStartSocket"));
	OutEndSocket = FName(TEXT("TraceEndSocket"));

	if (MeleeWeaponMesh && MeleeWeaponMesh->GetStaticMesh())
	{
		TraceComponent = MeleeWeaponMesh;
		OutStartSocket = FName(TEXT("TraceStartSocket"));
		OutEndSocket = FName(TEXT("TraceEndSocket"));
	}
	if (!TraceComponent->DoesSocketExist(OutStartSocket) || !TraceComponent->DoesSocketExist(OutEndSocket)) return nullptr;

	return TraceComponent;
}

void ATakenBase::StartMeleeTrace()
{
	HitActorsThisSwing.Empty();
	bMeleeTraceActive = true;
	
	FName StartSocket;
	FName EndSocket;
	USceneComponent* TraceComponent = GetTraceData(StartSocket, EndSocket);
	if (!TraceComponent) return;

	LastTraceStart = TraceComponent->GetSocketLocation(StartSocket);
	LastTraceEnd = TraceComponent->GetSocketLocation(EndSocket);
}

void ATakenBase::PerformMeleeTrace()
{

	FName StartSocket;
	FName EndSocket;
	USceneComponent* TraceComponent = GetTraceData(StartSocket, EndSocket);

	if (!TraceComponent) return;

	const FVector CurrentStart = TraceComponent->GetSocketLocation(StartSocket);
	const FVector CurrentEnd = TraceComponent->GetSocketLocation(EndSocket);

	if (bMeleeTraceActive)
	{
		TArray<FHitResult> Hits;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		GetWorld()->SweepMultiByChannel(
			Hits, LastTraceEnd, CurrentEnd, FQuat::Identity,
			ECC_Pawn, FCollisionShape::MakeSphere(15.f), Params);
		
		for (const FHitResult& Hit : Hits)
		{
			if (AActor* HitActor = Hit.GetActor())
			{
				if (HitActor->ActorHasTag(TEXT("Player")))
				{
					if (!HitActorsThisSwing.Contains(HitActor))
					{
						HitActorsThisSwing.Add(HitActor);
						UGameplayStatics::ApplyPointDamage(HitActor, AttackDamage, (CurrentEnd - LastTraceEnd).GetSafeNormal(), Hit, GetInstigatorController(), this, UDamageType::StaticClass());
					}
				}
				
			}
		}
	}

	LastTraceStart = CurrentStart;
	LastTraceEnd = CurrentEnd;
}

void ATakenBase::EndMeleeTrace()
{
	bMeleeTraceActive = false;
}


FName ATakenBase::ResolveReactionBone(FName HitBone)
{
	return HitBone.ToString().Contains(TEXT("head")) ? FName("head") : FName("spine_01");
}

void ATakenBase::ProcessHit(FName HitBone, FVector HitDirection, FVector HitLocation)
{
	FVector DirectionToHit = HitLocation - GetActorLocation();

	float RightOffset = FVector::DotProduct(GetActorRightVector(), DirectionToHit);

	float SideMultiplier = (RightOffset > 0) ? 1.0f : -1.0f;

	if (HitBone.ToString().Contains(TEXT("head")))
	{
		ReactionBoneName = FName("head");

		ImpactDirection = FVector::CrossProduct(FVector::UpVector, HitDirection.GetSafeNormal());
	}
	else
	{
		ReactionBoneName = FName("spine_01"); 
		ImpactDirection = FVector(0.0f, 0.0f, SideMultiplier);
	}
	bIsFlinching = true;
}

void ATakenBase::TriggerDespawn()
{
	SetActorTickEnabled(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	BP_OnDespawn();
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
			if (GetMesh() && GetMesh()->GetSkeletalMeshAsset())
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
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
    
		FName HitBone = PointDamageEvent->HitInfo.BoneName;
		FVector HitDirection = PointDamageEvent->ShotDirection;
		FVector HitLocation = PointDamageEvent->HitInfo.ImpactPoint;
		
		ProcessHit(HitBone, HitDirection, HitLocation); 
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



