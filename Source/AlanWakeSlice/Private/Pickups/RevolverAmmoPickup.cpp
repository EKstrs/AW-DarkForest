// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/RevolverAmmoPickup.h"

#include "Character/AlanWakeCharacter.h"
#include "Components/InventoryComponent.h"

ARevolverAmmoPickup::ARevolverAmmoPickup()
{
	ItemName = "Revolver ammo";
}

bool ARevolverAmmoPickup::CanInteract(class AAlanWakeCharacter* Player)
{
	if (Player && Player->InventoryComp)
	{
		return Player->InventoryComp->CanAddPickup(static_cast<uint8>(EPickupType::RevolverAmmo), AmmoAmount);
	}
	return false;
}

bool ARevolverAmmoPickup::ProcessPickup(class AAlanWakeCharacter* Player)
{
	if (Player && Player->InventoryComp)
	{
		return Player->InventoryComp->AddPickup(static_cast<uint8>(EPickupType::RevolverAmmo), AmmoAmount);
	}
	return false;
}
