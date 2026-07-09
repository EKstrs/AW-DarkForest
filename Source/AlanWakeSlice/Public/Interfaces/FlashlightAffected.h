// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FlashlightAffected.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFlashlightAffected : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ALANWAKESLICE_API IFlashlightAffected
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void ReceiveFlashlightExposure(float ExposureAmount, bool bIsFocusBeam, const FVector& HitLocation, const FVector& HitNormal) = 0;
};
