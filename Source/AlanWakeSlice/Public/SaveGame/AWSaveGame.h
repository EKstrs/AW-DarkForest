// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/AWGameMode.h"
#include "GameFramework/SaveGame.h"
#include "AWSaveGame.generated.h"

enum class EStoryBeat : uint8;
/**
 * 
 */
UCLASS()
class ALANWAKESLICE_API UAWSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UAWSaveGame();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	FVector PlayerLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	FRotator PlayerRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	int32 RevolverMag;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	int32 RevolverReserve;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	int32 FlareGunAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	int32 FlareCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	float BatteryLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	float PlayerHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	TArray<FName> CollectedManuscriptPages;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	TArray<FName> CollectedPickupIDs;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	EStoryBeat CurrentBeat;
	
};
