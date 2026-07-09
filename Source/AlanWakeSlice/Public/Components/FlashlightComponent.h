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
	
	UPROPERTY(EditAnywhere, Category = "FlashlightRadius")
	float FocusDrainMultiplier = 3.0f;

	UPROPERTY(EditAnywhere, Category = "FlashlightRadius")
	float AmbientDrainRate = 0.00f;

	UPROPERTY(EditAnywhere, Category = "FlashlightRadius")
	float InnerInnerConeAngle = 6.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightRadius")
	float InnerOuterConeAngle = 14.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightRadius")
	float OuterOuterConeAngle = 35.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightRadius")
	float InnerIntensity = 4000.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightRadius")
	float OuterIntensity = 400.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightRadius")
	float FocusInnerIntensity = 6000.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightRadius")
	float FocusOuterIntensity = 300.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightRadius")
	float FocusInnerInnerCone = 4.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightRadius")
	float FocusInnerOuterCone = 8.f;

	UPROPERTY(EditAnywhere, Category = "FlashlightRadius")
	float FocusOuterOuterCone = 20.f;

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
