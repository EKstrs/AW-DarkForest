//Written by Eetu Karhinen

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AlanWakeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UAnimMontage;
struct FInputActionValue;

UENUM(BlueprintType)
enum class EEquipState: uint8
{
	Unarmed UMETA(DisplayName = "Unarmed"),
	Throwable UMETA(DisplayName = "Throwable"),
	Pistol UMETA(DisplayName = "Pistol")
};


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

//Delegates

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipStateChanged, EEquipState, NewState);
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
	
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

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

    /** Weapon Reload Input Action*/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* ReloadAction;
    
    /** Interact Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* InteractAction;
    
public:
	AAlanWakeCharacter();
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


	/** DELEGATES */
	UPROPERTY(BlueprintAssignable, Category = "Weapons")
	FOnEquipStateChanged OnEquipStateChangedDelegate;
	

	/** Equipped items*/

	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UStaticMeshComponent* HandItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	EEquipState CurrentEquipState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	EEquipState PendingEquipState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* EquipMontage;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void FinalizeEquip();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bHasThrowable;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AActor> ThrowableClass;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EquipItem(EEquipState NewState);

	/** Throwables*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
	UAnimMontage* ThrowMontage;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ReleaseThrowable();
	
	/** Aiming */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAiming = false;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bCanAim = true;
	
protected:

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
	UAnimMontage* ReloadMontage;

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapons")
	void UpdateWeaponVisibility(EEquipState NewState);


	// Ammo System
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Pistol")
	int32 CurrentAmmo = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Pistol")
	int32 ReserveAmmo = 14;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Pistol")
	float BaseDamage = 35.0f;
	
	UPROPERTY(BlueprintReadWrite, Category = "Combat|Pistol")
	bool bCanFire = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Pistol")
	float FireRate = 0.2f;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat|Pistol")
	void BP_OnWeaponFired(bool bHitFound, AActor* HitActor, FVector TraceEndLocation, FVector HitNormal, FName HitBoneName);


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

