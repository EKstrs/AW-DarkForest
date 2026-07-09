// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AWPlayerController.generated.h"

class AAlanWakeCharacter;
class UInputMappingContext;
struct FInputActionValue;
class UInputAction;
/**
 * 
 */
UCLASS()
class ALANWAKESLICE_API AAWPlayerController : public APlayerController
{
	GENERATED_BODY()

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;
	
	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;
	
	/** Throw Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ThrowAction;
	
	/** Radial menu Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RadialMenuAction;

	/** Weapon fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireWeaponAction;

	/** Weapon fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FlashlightAction;

	/** Weapon Reload Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;
    
	/** Interact Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	
protected:
	virtual void SetupInputComponent() override;
    virtual void BeginPlay() override;

	UPROPERTY()
	AAlanWakeCharacter* AWCharacter;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	void HandleMove(const FInputActionValue& Value);
	void HandleSprintStart(const FInputActionValue& Value);
	void HandleSprintEnd(const FInputActionValue& Value);
	void HandleJump(const FInputActionValue& Value);
	void HandleStopJump(const FInputActionValue& Value);
	void HandleFire(const FInputActionValue& Value);
	void HandleFlashlightToggle(const FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);
	void HandleStartAim(const FInputActionValue& Value);
	void HandleStopAim(const FInputActionValue& Value);
	void HandleCrouch(const FInputActionValue& Value);
	void HandleThrow(const FInputActionValue& Value);
	void HandleReload(const FInputActionValue& Value);
	void HandleInteract(const FInputActionValue& Value);
};