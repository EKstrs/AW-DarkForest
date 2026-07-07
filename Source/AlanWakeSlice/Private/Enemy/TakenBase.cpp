// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/TakenBase.h"

#include "TimerManager.h"
#include "AI/TakenAIController.h"
#include "Components/DarknessShield.h"
#include "Engine/World.h"
#include "Game/AWGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"

ATakenBase::ATakenBase()
{
	PrimaryActorTick.bCanEverTick = false;
	DarknessShieldComponent = CreateDefaultSubobject<UDarknessShield>(TEXT("DarknessShieldComponent"));
	AIControllerClass = ATakenAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

}

void ATakenBase::ReceiveFlashlightExposure(float ExposureValue, bool bIsFocusBeam)
{
	if (DarknessShieldComponent)
	{
		DarknessShieldComponent->ProcessExposure(ExposureValue, bIsFocusBeam);
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
		//TODO Trigger dissolve VFX
		Destroy(); 
	}

	return ActualDamage;
}

void ATakenBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATakenBase::OnShieldDestroyed()
{
	//TODO Trigger VFX/AUDIO in BP
}

void ATakenBase::ResetSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	bHasFlinched = false;
	bIsBeingFocused = false;
}



