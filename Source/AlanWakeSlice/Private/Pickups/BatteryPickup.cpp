// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/BatteryPickup.h"

#include "Character/AlanWakeCharacter.h"
#include "Components/InventoryComponent.h"

ABatteryPickup::ABatteryPickup()
{
	ItemName = TEXT("Lithium Battery");
}

bool ABatteryPickup::CanInteract(class AAlanWakeCharacter* Player)
{
	if (Player && Player->InventoryComp)
	{
		return Player->InventoryComp->CanAddPickup(static_cast<uint8>(EPickupType::Battery), BatteryAmount);
	}
	return false;
}

bool ABatteryPickup::ProcessPickup(class AAlanWakeCharacter* Player)
{
	if (Player && Player->InventoryComp)
	{
		return Player->InventoryComp->AddPickup(static_cast<uint8>(EPickupType::Battery), BatteryAmount);
	}

	return false;
}
