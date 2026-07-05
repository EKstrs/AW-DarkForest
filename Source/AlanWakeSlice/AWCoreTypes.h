#pragma once

#include "CoreMinimal.h"
#include "AWCoreTypes.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None        UMETA(DisplayName = "None"),
	Revolver    UMETA(DisplayName = "Revolver"),
	FlareGun    UMETA(DisplayName = "Flare Gun"),
	Fist        UMETA(DisplayName = "Fist")
};
UENUM(BlueprintType)
enum class EPickupType : uint8
{
	RevolverAmmo,
	FlareGunAmmo,
	Battery,
	Flare
};


