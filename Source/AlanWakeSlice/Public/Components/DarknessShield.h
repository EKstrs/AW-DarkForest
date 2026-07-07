// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DarknessShield.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDarknessShieldDepletedSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALANWAKESLICE_API UDarknessShield : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDarknessShield();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DarknessShield")
	float MaxShield = 100.f;

	UPROPERTY(BlueprintReadOnly, Category = "DarknessShield")
	float CurrentShield = MaxShield;

	UPROPERTY(BlueprintReadOnly, Category = "DarknessShield")
	bool bIsVulnerable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DarknessShield")
	bool bRequiresFocusBeam = false;

	UPROPERTY(BlueprintAssignable, Category = "DarknessShield")
	FOnDarknessShieldDepletedSignature OnDarknessShieldDepletedDelegate;
	
	void ProcessExposure(float ExposureValue, bool bIsFocusBeam);

protected:
	virtual void BeginPlay() override;




		
};
