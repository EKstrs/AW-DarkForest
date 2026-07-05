// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AWPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/AlanWakeCharacter.h"
#include "Engine/LocalPlayer.h"

void AAWPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
    
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAWPlayerController::HandleMove);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AAWPlayerController::HandleSprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed,  this, &AAWPlayerController::HandleSprintEnd);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AAWPlayerController::HandleJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AAWPlayerController::HandleStopJump);
		EnhancedInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Started, this, &AAWPlayerController::HandleFire);
		EnhancedInputComponent->BindAction(FlashlightAction, ETriggerEvent::Started, this, &AAWPlayerController::HandleFlashlightToggle);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAWPlayerController::HandleLook);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this,	&AAWPlayerController::HandleStartAim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this,  &AAWPlayerController::HandleStopAim);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this,  &AAWPlayerController::HandleCrouch);
		EnhancedInputComponent->BindAction(ThrowAction,  ETriggerEvent::Started, this, &AAWPlayerController::HandleThrow);
		EnhancedInputComponent->BindAction(ReloadAction,  ETriggerEvent::Started, this, &AAWPlayerController::HandleReload);
	}
}

void AAWPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
	ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void AAWPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AWCharacter = Cast<AAlanWakeCharacter>(InPawn);
}

void AAWPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
}

void AAWPlayerController::HandleMove(const FInputActionValue& Value)
{
	if (AWCharacter)
	{
		AWCharacter->Move(Value); 
	}
}

void AAWPlayerController::HandleSprintStart(const FInputActionValue& Value)
{
	if (AWCharacter)
	{
		AWCharacter->SprintStart(Value); 
	}
}

void AAWPlayerController::HandleSprintEnd(const FInputActionValue& Value)
{
	if (AWCharacter)
	{
		AWCharacter->SprintEnd(Value); 
	}
}

void AAWPlayerController::HandleJump(const FInputActionValue& Value)
{
	if (AWCharacter)
	{
		AWCharacter->Jump();
	}
}

void AAWPlayerController::HandleStopJump(const FInputActionValue& Value)
{
	if (AWCharacter)
	{
		AWCharacter->StopJumping();
	}
}

void AAWPlayerController::HandleFire(const FInputActionValue& Value)
{
	if (AWCharacter)
	{
		AWCharacter->FireWeapon(Value);
	}
}

void AAWPlayerController::HandleFlashlightToggle(const FInputActionValue& Value)
{
	if (AWCharacter)
	{
		AWCharacter->ToggleFlashlight(Value);
	}
}

void AAWPlayerController::HandleLook(const FInputActionValue& Value)
{
	if (AWCharacter)
	{
		AWCharacter->Look(Value);
	}
}

void AAWPlayerController::HandleStartAim(const FInputActionValue& Value)
{
	if (AWCharacter)
	{
		AWCharacter->StartAim(Value);
	}
}

void AAWPlayerController::HandleStopAim(const FInputActionValue& Value)
{
	if (AWCharacter)
	{
		AWCharacter->StopAim(Value);
	}
}


void AAWPlayerController::HandleCrouch(const FInputActionValue& Value)
{
	if (AWCharacter)
	{
		AWCharacter->ToggleCrouch(Value);
	}
}

void AAWPlayerController::HandleThrow(const FInputActionValue& Value)
{
	if (AWCharacter)
	{
		AWCharacter->ThrowItem(Value);
	}
}


void AAWPlayerController::HandleReload(const FInputActionValue& Value)
{
	if (AWCharacter)
	{
		AWCharacter->Reload(Value);
	}
}

