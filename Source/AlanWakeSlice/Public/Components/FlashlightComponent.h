// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlashlightComponent.generated.h"


class USpotLightComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALANWAKESLICE_API UFlashlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFlashlightComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	bool FlashlightState() const { return bIsOn; }

	void SetupLight(USpotLightComponent* LightSubcomponent);
	void ToggleFlashlight();
	void SetFocusMode(bool bEnabled);
	float GetExposureOnActor(AActor* TargetActor);
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	USpotLightComponent* SpotLight;
	
	bool bIsOn = false;
	bool bFocusMode = false;
	float BatteryLevel = 1.0f;
	float FocusDrainMultiplier = 3.0f;
		
};
