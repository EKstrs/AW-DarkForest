// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "AlanWakeSlice/AWCoreTypes.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnWeaponFiredSignature, bool, bHit, AActor*, HitActor, FVector, HitLocation, FVector, HitNormal);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALANWAKESLICE_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeaponComponent();

	void Initialize(UInventoryComponent* InInventoryComponent);
	void EquipWeapon(EWeaponType NewWeapon);
	bool Fire(FVector CameraLocation, FVector CameraForward);
	void Reload();

	UFUNCTION(BLueprintCallable)
	EWeaponType GetCurrentWeapon() const { return CurrentWeapon; }

	
	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnWeaponFiredSignature OnWeaponFiredDelegate;
	
protected:

	virtual void BeginPlay() override;

	
private:
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;
	int32 MaxRevolverMag = 6;
	float BaseDamage = 35.f;
	EWeaponType CurrentWeapon = EWeaponType::None;
};
