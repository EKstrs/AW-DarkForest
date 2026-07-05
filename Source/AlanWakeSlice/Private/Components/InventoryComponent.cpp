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

void UInventoryComponent::AddPickup(uint8 PickupType, int32 Amount)
{
	EPickupType Type = static_cast<EPickupType>(PickupType);

	switch (Type)
	{
	case EPickupType::RevolverAmmo:
		RevolverReserveAmmo += Amount; // Adds to the reserve, not the active magazine
		break;
	case EPickupType::FlareGunAmmo:
		FlareGunReserveAmmo += Amount;
		break;
	case EPickupType::Battery:
		BatteryReserve += Amount;
		break;
	case EPickupType::Flare:
		FlareCount += Amount;
		break;
	}

	// Tell the HUD to update!
	OnInventoryChangedDelegate.Broadcast();
}

