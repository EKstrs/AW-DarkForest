// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlashlightComponent.generated.h"


class UCameraComponent;
class USpotLightComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBatteryDepleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBatteryLevelChanged, float, NewLevel);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALANWAKESLICE_API UFlashlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFlashlightComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	float GetBatteryLevel() const { return BatteryLevel; }

	USpotLightComponent* GetInnerLight() const { return SpotLightInner; }

	UFUNCTION(BlueprintCallable)
	void SetBatteryLevel(float NewLevel) 
	{ 
		BatteryLevel = NewLevel; 
		OnBatteryLevelChanged.Broadcast(BatteryLevel); // Update the HUD!
	}

	UFUNCTION(BlueprintCallable)
	bool IsOn() const { return bIsOn; }

	UPROPERTY(BlueprintAssignable)
	FOnBatteryDepleted OnBatteryDepleted;

	UPROPERTY(BlueprintAssignable)
	FOnBatteryLevelChanged OnBatteryLevelChanged;
	
	void SetupLights(USpotLightComponent* Inner, USpotLightComponent* Outer);
	void SetCamera(UCameraComponent* InCamera);
	void ToggleFlashlight();
	void SetFocusMode(bool bEnabled);
	float GetExposureOnActor(AActor* TargetActor, FVector& OutHitLocation, FVector& OutHitNormal);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlashlightExposure")
	float BaseShieldDrainRate = 50.f;
	
	UPROPERTY(EditAnywhere, Category = "FlashlightExposure")
	float FocusDrainMultiplier = 3.0f;

	UPROPERTY(EditAnywhere, Category = "FlashlightExposure")
	float AmbientDrainRate = 0.00f;

	UPROPERTY(EditAnywhere, Category = "FlashlightCoreSettings")
	float CoreInnerConeAngle = 6.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightCoreSettings")
	float CoreOuterConeAngle = 14.f;
	
	UPROPERTY(EditAnywhere, Category = "FlashlightSpillSettings")
    float SpillInnerConeAngle = 35.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightSpillSettings")
	float SpillOuterConeAngle = 35.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightCoreSettings")
	float CoreIntensity = 4000.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightSpillSettings")
	float SpillIntensity = 400.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightCoreSettings")
	float FocusCoreIntensity = 6000.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightSpillSettings")
	float FocusSpillIntensity = 300.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightCoreSettings")
	float FocusCoreInnerCone = 4.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightCoreSettings")
	float FocusCoreOuterCone = 8.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightSpillSettings")
	float FocusSpillInnerCone = 20.f;
	
	UPROPERTY(EditAnywhere, Category = "FlashlightSpillSettings")
	float FocusSpillOuterCone = 20.f;
	
	UPROPERTY(EditAnywhere, Category = "FlashlightCoreSettings")
	float CoreVolumetricScattering = 0.5f;

	UPROPERTY(EditAnywhere, Category = "FlashlightSpillSettings")
	float SpillVolumetricScattering = 0.5f;

	UPROPERTY(EditAnywhere, Category = "FlashlightCoreSettings")
	float FocusCoreVolumetricScattering = 0.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightSpillSettings")
	float FocusSpillVolumetricScattering = 0.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightCoreSettings")
	float CoreAttenuationRadius = 2000.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightSpillSettings")
	float SpillAttenuationRadius = 2000.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightCoreSettings")
	float FocusCoreAttenuationRadius = 2000.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightSpillSettings")
	float FocusSpillAttenuationRadius = 2000.f;
	

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	TObjectPtr<USpotLightComponent> SpotLightInner;

	UPROPERTY()
	TObjectPtr<USpotLightComponent> SpotLightOuter;

	UPROPERTY()
	TObjectPtr<UCameraComponent> CameraComponent;
	
	bool bIsOn = false;
	bool bFocusMode = false;
	float BatteryLevel = 1.0f;

	void UpdateBatteryDrain(float DeltaTime);
	void UpdateFlashlightAimDirection();	
};
