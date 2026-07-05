// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/FlashlightComponent.h"

#include "CollisionQueryParams.h"
#include "Components/SpotLightComponent.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"

UFlashlightComponent::UFlashlightComponent()
{

	PrimaryComponentTick.bCanEverTick = true;
}

void UFlashlightComponent::BeginPlay()
{
	Super::BeginPlay();

}


void UFlashlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bIsOn)
	{
		float Drain = bFocusMode ? (DeltaTime * FocusDrainMultiplier * 0.05f) : (DeltaTime * 0.01f);
		BatteryLevel = FMath::Clamp(BatteryLevel - Drain, 0.f, 1.f);

		if (BatteryLevel <= 0.f)
		{
			bIsOn = false;
			if (SpotLight) SpotLight->SetVisibility(false);
		}
	}
}

void UFlashlightComponent::SetupLight(USpotLightComponent* LightSubcomponent)
{
	SpotLight = LightSubcomponent;
	if (SpotLight) SpotLight->SetVisibility(false);
}

void UFlashlightComponent::ToggleFlashlight()
{
	bIsOn = !bIsOn;
	if (SpotLight) SpotLight->SetVisibility(bIsOn);
}

void UFlashlightComponent::SetFocusMode(bool bEnabled)
{
	bFocusMode = bEnabled;
	if (SpotLight && bIsOn)
	{
		SpotLight->InnerConeAngle = bFocusMode ? 8.f : 20.f;
		SpotLight->Intensity = bFocusMode ? 5000.f : 1500.f;
	}
}

float UFlashlightComponent::GetExposureOnActor(AActor* TargetActor)
{
	if (!bIsOn || !SpotLight || !TargetActor) return 0.0f;

	FVector LightLocation = SpotLight->GetComponentLocation();
	FVector LightForward = SpotLight->GetForwardVector();
	FVector DirToTarget = (TargetActor->GetActorLocation() - LightLocation).GetSafeNormal();
    
	float DotProduct = FVector::DotProduct(LightForward, DirToTarget);
	float OuterConeRad = FMath::DegreesToRadians(SpotLight->OuterConeAngle);
    
	if (DotProduct > FMath::Cos(OuterConeRad))
	{
		// Add line trace — don't expose through walls
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetOwner());
		Params.AddIgnoredActor(TargetActor);
        
		bool bBlocked = GetWorld()->LineTraceSingleByChannel(
			Hit, LightLocation, TargetActor->GetActorLocation(), ECC_Visibility, Params);
        
		if (!bBlocked)
			return bFocusMode ? 1.0f : 0.25f;
	}
	return 0.0f;
}

