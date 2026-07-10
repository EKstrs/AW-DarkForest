// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DarknessShield.h"


UDarknessShield::UDarknessShield()
{

	PrimaryComponentTick.bCanEverTick = false;

}

void UDarknessShield::BeginPlay()
{
	Super::BeginPlay();

	
	CurrentShield = MaxShield;
	bIsVulnerable = (MaxShield <= 0.0f);
	
}

void UDarknessShield::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void UDarknessShield::ProcessExposure(float ExposureValue, bool bIsFocusBeam)
{
	if (bIsVulnerable) return;

	if (bRequiresFocusBeam && !bIsFocusBeam) return;

	CurrentShield = FMath::Clamp(CurrentShield - ExposureValue, 0.0f, MaxShield);
	OnShieldValueChangedDelegate.Broadcast(CurrentShield / MaxShield);

	if (CurrentShield <= 0.0f)
	{
		bIsVulnerable = true;
		OnDarknessShieldDepletedDelegate.Broadcast();
	}
}
