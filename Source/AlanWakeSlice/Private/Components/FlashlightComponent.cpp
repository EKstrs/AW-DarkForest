// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/FlashlightComponent.h"

#include "CollisionQueryParams.h"
#include "Animation/AnimInstanceProxy.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SpotLightComponent.h"
#include "Engine/HitResult.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Interfaces/FlashlightAffected.h"
#include "Kismet/KismetSystemLibrary.h"

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
	if (!SpotLightInner || !CameraComponent) return;

	// Find the point the camera (and crosshair) is actually looking at, rather than
	// just copying the camera's rotation. The spotlight sits on the hand socket, offset
	// from the camera in world space, so matching rotation alone causes the beam to
	// diverge from the crosshair the further out it goes (classic parallax problem).
	const FVector CameraLoc = CameraComponent->GetComponentLocation();
	const FVector CameraFwd = CameraComponent->GetForwardVector();
	const FVector TraceEnd = CameraLoc + (CameraFwd * 5000.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	FVector AimPoint = TraceEnd;
	if (GetWorld()->LineTraceSingleByChannel(Hit, CameraLoc, TraceEnd, ECC_Visibility, Params))
	{
		AimPoint = Hit.ImpactPoint;
	}

	// Aim the spotlight AT that point instead of matching camera rotation directly.
	const FVector LightLoc = SpotLightInner->GetComponentLocation();
	const FRotator TargetRotation = (AimPoint - LightLoc).Rotation();

	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	const FRotator SmoothRotation = FMath::RInterpTo(SpotLightInner->GetComponentRotation(), TargetRotation, DeltaTime, 15.0f);

	SpotLightInner->SetWorldRotation(SmoothRotation);
	if (SpotLightOuter) SpotLightOuter->SetWorldRotation(SmoothRotation);
}


void UFlashlightComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bIsOn)
	{
		UpdateBatteryDrain(DeltaTime);
		UpdateFlashlightAimDirection();

		FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECC_Pawn);
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetOwner());

		FCollisionShape SphereShape = FCollisionShape::MakeSphere(2000.f);
		TArray<FOverlapResult> OverlapResults;

		if (GetWorld()->OverlapMultiByObjectType(OverlapResults, SpotLightInner->GetComponentLocation(), FQuat::Identity, ObjParams, SphereShape, Params))
		{
			for (const FOverlapResult& Result : OverlapResults)
			{
				AActor* OverlappedActor = Result.GetActor();

				if (IFlashlightAffected* LightTarget = Cast<IFlashlightAffected>(OverlappedActor))
				{
					FVector HitLocation, HitNormal;
					float Exposure = GetExposureOnActor(OverlappedActor, HitLocation, HitNormal);

					if (Exposure > 0.0f)
					{
						LightTarget->ReceiveFlashlightExposure(Exposure * BaseShieldDrainRate * DeltaTime, bFocusMode, HitLocation, HitNormal);
					}
				} 
			}
		}
	}
}

void UFlashlightComponent::SetupLights(USpotLightComponent* Inner, USpotLightComponent* Outer)
{
	SpotLightInner = Inner;
	SpotLightOuter = Outer;

	if (SpotLightInner)
	{
		SpotLightInner->SetInnerConeAngle(CoreInnerConeAngle);
		SpotLightInner->SetOuterConeAngle(CoreOuterConeAngle);
		SpotLightInner->SetIntensity(CoreIntensity);
		SpotLightInner->SetVolumetricScatteringIntensity(CoreVolumetricScattering);
		SpotLightInner->SetAttenuationRadius(CoreAttenuationRadius);
		SpotLightInner->SetVisibility(false);
	}

	if (SpotLightOuter)
	{
		SpotLightOuter->SetInnerConeAngle(SpillInnerConeAngle);
		SpotLightOuter->SetOuterConeAngle(SpillOuterConeAngle);
		SpotLightOuter->SetIntensity(SpillIntensity);
		SpotLightOuter->SetVolumetricScatteringIntensity(SpillVolumetricScattering);
		SpotLightOuter->SetAttenuationRadius(SpillAttenuationRadius);
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
	if (!bIsOn) return;
	bFocusMode = bEnabled;
	if (bFocusMode)
	{
		if (SpotLightInner)
		{
			SpotLightInner->SetInnerConeAngle(FocusCoreInnerCone);
			SpotLightInner->SetOuterConeAngle(FocusCoreOuterCone);
			SpotLightInner->SetIntensity(FocusCoreIntensity);
			SpotLightInner->SetVolumetricScatteringIntensity(FocusCoreVolumetricScattering);
			SpotLightInner->SetAttenuationRadius(FocusCoreAttenuationRadius);
		}
		if (SpotLightOuter)
		{
			SpotLightOuter->SetInnerConeAngle(FocusSpillInnerCone);
			SpotLightOuter->SetOuterConeAngle(FocusSpillOuterCone);
			SpotLightOuter->SetIntensity(FocusSpillIntensity);
			SpotLightOuter->SetVolumetricScatteringIntensity(FocusSpillVolumetricScattering);
			SpotLightOuter->SetAttenuationRadius(FocusSpillAttenuationRadius);
		}
	}
	else
	{
		if (SpotLightInner)
		{
			SpotLightInner->SetInnerConeAngle(CoreInnerConeAngle);
			SpotLightInner->SetOuterConeAngle(CoreOuterConeAngle);
			SpotLightInner->SetIntensity(CoreIntensity);
			SpotLightInner->SetVolumetricScatteringIntensity(CoreVolumetricScattering);
			SpotLightInner->SetAttenuationRadius(CoreAttenuationRadius);
		}
		if (SpotLightOuter)
		{
			SpotLightOuter->SetInnerConeAngle(SpillInnerConeAngle);
			SpotLightOuter->SetOuterConeAngle(SpillOuterConeAngle);
			SpotLightOuter->SetIntensity(SpillIntensity);
			SpotLightOuter->SetVolumetricScatteringIntensity(SpillVolumetricScattering);
			SpotLightOuter->SetAttenuationRadius(SpillAttenuationRadius);
		}

		if (SpotLightInner) SpotLightInner->SetUsingAbsoluteRotation(false);
		if (SpotLightOuter) SpotLightOuter->SetUsingAbsoluteRotation(false);
	}
}

float UFlashlightComponent::GetExposureOnActor(AActor* TargetActor, FVector& OutHitLocation, FVector& OutHitNormal)
{
	if (!bIsOn || !SpotLightInner || !TargetActor) return 0.0f;

	FVector LightLoc = SpotLightInner->GetComponentLocation();
	FVector LightFwd = SpotLightInner->GetForwardVector();

	FVector TargetChestLoc = TargetActor->GetActorLocation();
	float TargetRadius = 40.0f;
	
	if (ACharacter* CharTarget = Cast<ACharacter>(TargetActor))
	{
		TargetChestLoc.Z += CharTarget->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.5f;
		TargetRadius = CharTarget->GetCapsuleComponent()->GetScaledCapsuleRadius();
	}
	
	FVector VectorToTarget = TargetChestLoc - LightLoc;
	float Distance = VectorToTarget.Size();
	FVector DirToTarget = VectorToTarget / FMath::Max(Distance, 1.0f);
	
	float Dot = FVector::DotProduct(LightFwd, DirToTarget);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	Params.AddIgnoredActor(TargetActor);

	bool bBlocked = GetWorld()->LineTraceSingleByChannel(
		Hit, LightLoc, TargetChestLoc, ECC_Visibility, Params);

	if (bBlocked) return 0.0f;

	float AngularRadius = FMath::Asin(FMath::Clamp(TargetRadius / FMath::Max(Distance, 1.0f), 0.0f, 1.0f));

	float CurrentConeRad = FMath::DegreesToRadians(SpotLightInner->OuterConeAngle);

	float TotalAllowedAngle = CurrentConeRad + AngularRadius;

	if (FMath::Acos(Dot) <= TotalAllowedAngle)
	{
		OutHitLocation = TargetChestLoc;
		OutHitNormal = -LightFwd;
		return bFocusMode ? 1.0f : 0.6f;
	}
	return 0.0f;
}
