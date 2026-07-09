// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/PickupBase.h"
#include "RevolverAmmoPickup.generated.h"

/**
 * 
 */
UCLASS()
class ALANWAKESLICE_API ARevolverAmmoPickup : public APickupBase
{
	GENERATED_BODY()

public:
	ARevolverAmmoPickup();

	virtual bool CanInteract(class AAlanWakeCharacter* Player) override;

protected:
	virtual bool ProcessPickup(class AAlanWakeCharacter* Player) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Setup")
	int32 AmmoAmount = 6;
};
