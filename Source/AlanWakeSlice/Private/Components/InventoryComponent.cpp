// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

bool UInventoryComponent::TryConsumeAmmo(EWeaponType WeaponType)
{
	if (WeaponType == EWeaponType::Revolver && RevolverAmmo > 0)
	{
		RevolverAmmo--;
		OnInventoryChangedDelegate.Broadcast();
		return true;
	}
	if (WeaponType == EWeaponType::FlareGun && FlareGunAmmo > 0)
	{
		FlareGunAmmo--;
		OnInventoryChangedDelegate.Broadcast();
		return true;
	}
	return false;
}

void UInventoryComponent::ReloadWeapon(EWeaponType WeaponType, int32 MaxMagSize)
{
	if (WeaponType == EWeaponType::Revolver)
	{
		int32 AmmoNeeded = MaxMagSize - RevolverAmmo;
		int32 AmmoToMove = FMath::Min(AmmoNeeded, RevolverReserveAmmo);
		RevolverAmmo += AmmoToMove;
		RevolverReserveAmmo -= AmmoToMove;
		OnInventoryChangedDelegate.Broadcast();
	}
}

bool UInventoryComponent::AddPickup(uint8 PickupType, int32 Amount)
{
	EPickupType Type = static_cast<EPickupType>(PickupType);
	bool bWasAdded = false;

	switch (Type)
	{
	case EPickupType::RevolverAmmo:
	    if(RevolverReserveAmmo < MaxRevolverReserveAmmo)
	    {
	    	RevolverReserveAmmo = FMath::Min(RevolverReserveAmmo + Amount, MaxRevolverReserveAmmo);
            bWasAdded = true;
	    }
		break;
	case EPickupType::FlareGunAmmo:
		if (FlareGunReserveAmmo < MaxFlareGunReserveAmmo)
		{
			FlareGunReserveAmmo = FMath::Min(FlareGunReserveAmmo + Amount, MaxFlareGunReserveAmmo);
			bWasAdded = true;
		}
		break;
	case EPickupType::Battery:
		if (BatteryReserve < MaxBatteryReserve)
		{
			BatteryReserve = FMath::Min(BatteryReserve + Amount, MaxBatteryReserve);
			bWasAdded = true;
		}
		break;
	case EPickupType::Flare:
		if (FlareCount < MaxFlareCount)
		{
			FlareCount = FMath::Min(FlareCount + Amount, MaxFlareCount);
			bWasAdded = true;
		}
		break;
	}

	if (bWasAdded)
	{
		OnInventoryChangedDelegate.Broadcast();
	}

	return bWasAdded;
}

bool UInventoryComponent::CanAddPickup(uint8 PickupType, int32 Amount) const
{
	EPickupType Type = static_cast<EPickupType>(PickupType);

	switch (Type)
	{
	case EPickupType::RevolverAmmo:
		return RevolverReserveAmmo < MaxRevolverReserveAmmo;
	case EPickupType::Battery:
		return BatteryReserve < MaxBatteryReserve;
	case EPickupType::Flare:
		return FlareCount < MaxFlareCount;
	default:
		return false;
	}
}

