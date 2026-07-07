//Written by Eetu Karhinen

#pragma once

#include "CoreMinimal.h"
#include "AlanWakeSlice/AWCoreTypes.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AlanWakeCharacter.generated.h"

class USpotLightComponent;
class UFlashlightComponent;
class UWeaponComponent;
class UInventoryComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UAnimMontage;
struct FInputActionValue;


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);

UCLASS(config=Game)
class AAlanWakeCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	
public:
	AAlanWakeCharacter();
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void LoadPlayerState();
	//Getters

	float GetHealth() const { return CurrentHealth; }
	float GetMaxHealth() const { return MaxHealth; }

	/** Components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* InventoryComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UWeaponComponent* WeaponComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UFlashlightComponent* FlashlightComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* FlashlightMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpotLightComponent* FlashlightInner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpotLightComponent* FlashlightOuter;

	/** Equipped items*/

	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UStaticMeshComponent* HandItemMesh;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* EquipMontage;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EquipWeapon(EWeaponType NewWeaponType);
	
	/** Aiming */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAiming = false;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bCanAim = true;
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void SprintStart(const FInputActionValue& Value);

	void SprintEnd(const FInputActionValue& Value);

	/** Called for crouching input */
	void ToggleCrouch(const FInputActionValue& Value );

	/** Called for throwing input */
	void ThrowItem(const FInputActionValue& value);

	/** Called for Aiming input */
	void StartAim(const FInputActionValue& value);
	void StopAim(const FInputActionValue& value);

	/** Called for Weapon fire input */
	void FireWeapon(const FInputActionValue& value);
	
	/** Called for Weapon reload input */
	void Reload(const FInputActionValue& value);

	void Interact(const FInputActionValue& value);

	void ToggleFlashlight(const FInputActionValue& value);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void BP_UpdateWeaponVisuals();

	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
	void BP_OnAimStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
	void BP_OnAimStopped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
	void BP_OnSprintStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
	void BP_OnSprintStopped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
	void BP_PlayDryFire();
	
protected:

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
	UAnimMontage* ReloadMontage;
	
	// Health system
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,  Category = "Health")
	float MaxHealth = 100.f;
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float CurrentHealth = MaxHealth;

	bool bIsDead = false;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(BlueprintAssignable, Category = "Weapons")
	FOnHealthChanged OnHealthChangedDelegate;
	
	void OnDeath();
private:
	bool bIsTurning = false;
	
};

