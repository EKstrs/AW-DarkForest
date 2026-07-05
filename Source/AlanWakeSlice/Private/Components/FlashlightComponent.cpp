// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/FlashlightComponent.h"

#include "CollisionQueryParams.h"
#include "Camera/CameraComponent.h"
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

void UFlashlightComponent::UpdateBatteryDrain(float DeltaTime)
{
	float Drain = bFocusMode
		? (DeltaTime * FocusDrainMultiplier * 0.01f)
		: (DeltaTime * AmbientDrainRate);

	BatteryLevel = FMath::Clamp(BatteryLevel - Drain, 0.f, 1.f);
	OnBatteryLevelChanged.Broadcast(BatteryLevel);

	if (BatteryLevel <= 0.f)
	{
		bIsOn = false;
		if (SpotLightInner) SpotLightInner->SetVisibility(false);
		if (SpotLightOuter) SpotLightOuter->SetVisibility(false);
		OnBatteryDepleted.Broadcast();
	}
}

void UFlashlightComponent::UpdateFlashlightAimDirection()
{
	if (!bFocusMode || !SpotLightInner || !CameraComponent) return;

	FVector CamLoc = CameraComponent->GetComponentLocation();
	FVector CamForward = CameraComponent->GetForwardVector();

	// Trace from camera center to find world target point
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	FVector TraceEnd = CamLoc + (CamForward * 5000.f);
	FVector TargetPoint = TraceEnd;

	if (GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, TraceEnd, ECC_Visibility, Params))
		TargetPoint = Hit.ImpactPoint;

	// Rotate from spotlight's current world position toward the hit point
	FVector FromLight = SpotLightInner->GetComponentLocation();
	FRotator AimRotation = (TargetPoint - FromLight).GetSafeNormal().Rotation();

	SpotLightInner->SetWorldRotation(AimRotation);
	if (SpotLightOuter) SpotLightOuter->SetWorldRotation(AimRotation);
}


void UFlashlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bIsOn)
	{
		UpdateBatteryDrain(DeltaTime);
		UpdateFlashlightAimDirection();
	}
}

void UFlashlightComponent::SetupLights(USpotLightComponent* Inner, USpotLightComponent* Outer)
{
	SpotLightInner = Inner;
	SpotLightOuter = Outer;

	if (SpotLightInner)
	{
		SpotLightInner->SetInnerConeAngle(InnerInnerConeAngle);
		SpotLightInner->SetOuterConeAngle(InnerOuterConeAngle);
		SpotLightInner->SetIntensity(InnerIntensity);
		SpotLightInner->SetVisibility(false);
	}

	if (SpotLightOuter)
	{
		SpotLightOuter->SetInnerConeAngle(0.f);
		SpotLightOuter->SetOuterConeAngle(OuterOuterConeAngle);
		SpotLightOuter->SetIntensity(OuterIntensity);
		SpotLightOuter->SetVisibility(false);
	}
}

void UFlashlightComponent::SetCamera(UCameraComponent* InCamera)
{
	CameraComponent = InCamera;
}

void UFlashlightComponent::ToggleFlashlight()
{
	bIsOn = !bIsOn;

	if (SpotLightInner) SpotLightInner->SetVisibility(bIsOn);
	if (SpotLightOuter) SpotLightOuter->SetVisibility(bIsOn);
	if (!bIsOn && bFocusMode)
		bFocusMode = false;
}

void UFlashlightComponent::SetFocusMode(bool bEnabled)
{
	bFocusMode = bEnabled;
	if (!bIsOn) return;

	if (bFocusMode)
	{
		if (SpotLightInner)
		{
			SpotLightInner->SetInnerConeAngle(FocusInnerInnerCone);
			SpotLightInner->SetOuterConeAngle(FocusInnerOuterCone);
			SpotLightInner->SetIntensity(FocusInnerIntensity);
		}
		if (SpotLightOuter)
		{
			SpotLightOuter->SetOuterConeAngle(FocusOuterOuterCone);
			SpotLightOuter->SetIntensity(FocusOuterIntensity);
		}
	}
	else
	{
		if (SpotLightInner)
		{
			SpotLightInner->SetInnerConeAngle(InnerInnerConeAngle);
			SpotLightInner->SetOuterConeAngle(InnerOuterConeAngle);
			SpotLightInner->SetIntensity(InnerIntensity);
		}
		if (SpotLightOuter)
		{
			SpotLightOuter->SetOuterConeAngle(OuterOuterConeAngle);
			SpotLightOuter->SetIntensity(OuterIntensity);
		}
		
		if (SpotLightInner) SpotLightInner->SetUsingAbsoluteRotation(false);
		if (SpotLightOuter) SpotLightOuter->SetUsingAbsoluteRotation(false);
	}
}

float UFlashlightComponent::GetExposureOnActor(AActor* TargetActor)
{
	if (!bIsOn || !SpotLightInner || !TargetActor) return 0.0f;

	FVector LightLoc = SpotLightInner->GetComponentLocation();
	FVector LightFwd = SpotLightInner->GetForwardVector();
	FVector DirToTarget = (TargetActor->GetActorLocation() - LightLoc).GetSafeNormal();
	float Dot = FVector::DotProduct(LightFwd, DirToTarget);
	
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	Params.AddIgnoredActor(TargetActor);

	bool bBlocked = GetWorld()->LineTraceSingleByChannel(
		Hit, LightLoc, TargetActor->GetActorLocation(), ECC_Visibility, Params);

	if (bBlocked) return 0.0f;
	
	float InnerConeRad = FMath::DegreesToRadians(SpotLightInner->OuterConeAngle);
	if (Dot > FMath::Cos(InnerConeRad))
		return bFocusMode ? 1.0f : 0.6f;

	// Outer spill — weak exposure, only affects normal Taken shields
	// Heavy Taken (bRequiresFocusBeam) ignores this
	if (SpotLightOuter)
	{
		float OuterConeRad = FMath::DegreesToRadians(SpotLightOuter->OuterConeAngle);
		if (Dot > FMath::Cos(OuterConeRad))
			return 0.15f;
	}

	return 0.0f;

}

