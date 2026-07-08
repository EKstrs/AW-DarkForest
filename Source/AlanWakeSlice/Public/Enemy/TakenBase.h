// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/FlashlightAffected.h"
#include "TakenBase.generated.h"

class UDarknessShield;

UCLASS()
class ALANWAKESLICE_API ATakenBase : public ACharacter, public IFlashlightAffected
{
	GENERATED_BODY()

public:

	ATakenBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TakenBase")
	UDarknessShield* DarknessShieldComponent;
	
	virtual void ReceiveFlashlightExposure(float ExposureValue, bool bIsFocusBeam) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnFocusFlinch();

	bool bIsBeingFocused = false;
protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.f;
	float CurrentHealth = MaxHealth;

	UFUNCTION()
	void OnShieldDestroyed();

	UFUNCTION(BlueprintImplementableEvent, Category = "Audio")
	void BP_UpdateShieldAudio(float ShieldPercentage);

	UFUNCTION(BlueprintImplementableEvent, Category = "Audio")
	void BP_StopShieldAudio();
private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TakenBase", meta = (AllowPrivateAccess = "true"))
	float BaseWalkSpeed = 500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TakenBase",  meta = (AllowPrivateAccess = "true"))
	float FocusedWalkSpeed = 150.f;
	bool bHasFlinched = false;

	FTimerHandle SpeedResetTimer;

	void ResetSpeed();
};
