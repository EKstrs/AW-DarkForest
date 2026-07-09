// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AlanWakeSlice/AWCoreTypes.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALANWAKESLICE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UInventoryComponent();

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChangedDelegate;
	
	// Core Resource Storage
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetRevolverMag() const { return RevolverAmmo; }
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetRevolverReserve() const { return RevolverReserveAmmo; }
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetFlareGunAmmo() const { return FlareGunAmmo; }
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetFlareCount() const { return FlareCount; }
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	float GetBatteryReserve() const { return BatteryReserve; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	float GetMaxBatteryReserve() const { return MaxBatteryReserve; }

	//Setters
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetRevolverMag(int32 InMag) {  RevolverAmmo = InMag; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetRevolverReserve(int32 InReserve) {   RevolverReserveAmmo = InReserve; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetFlareGunAmmo(int32 InAmmo) { FlareGunAmmo =  InAmmo; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetFlareCount(int32 InCount) { FlareCount = InCount; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetBatteryReserve(float InReserve) {  BatteryReserve = InReserve; }

	bool TryConsumeAmmo(EWeaponType WeaponType);
	void ReloadWeapon(EWeaponType WeaponType, int32 MaxMagSize);
	bool AddPickup(uint8 PickupType, int32 Amount);

	bool CanAddPickup(uint8 PickupType, int32 Amount) const;


    //Max Capacities
    UPROPERTY(EditDefaultsOnly, Category = "Inventory Limits")
    int32 MaxRevolverReserveAmmo = 24;
    
    UPROPERTY(EditDefaultsOnly, Category = "Inventory Limits")
    float MaxBatteryReserve = 4.0f; 
    
    UPROPERTY(EditDefaultsOnly, Category = "Inventory Limits")
    int32 MaxFlareCount = 6;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory Limits")
	int32 MaxFlareGunReserveAmmo = 2;
    
protected:

	virtual void BeginPlay() override;

private:

	
	int32 RevolverAmmo = 6;
	int32 RevolverReserveAmmo = 12;
	
	int32 FlareGunAmmo = 0;
	int32 FlareGunReserveAmmo = 0;

	int32 FlareCount = 0;

	int32 ThrowableAmmo = 0;

	float BatteryReserve = 0.f;
};
