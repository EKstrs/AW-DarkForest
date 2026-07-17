// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AWGameMode.generated.h"



UENUM(BlueprintType)
enum class EStoryBeat : uint8
{
	None = 0,
	ArrivedAtCabin,
	InsideCabinLightsOff,
	GeneratorOn,
	InsideCabinLightsOn,
	Blackout,
	GeneratorFailed,
	InvestigatingScream,
	WreckFound,
	Encounter1Active,
	Encounter1Cleared,
	BridgeReached
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoryBeatChangedSignature, EStoryBeat, NewBeat);

class ATakenBase;
/**
 * 
 */
UCLASS()
class ALANWAKESLICE_API AAWGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story Progression")
    EStoryBeat CurrentBeat = EStoryBeat::ArrivedAtCabin;
    
   	UPROPERTY(BlueprintAssignable, Category = "Story Progression")
    FOnStoryBeatChangedSignature OnStoryBeatChanged;
    
    UFUNCTION(BlueprintCallable, Category = "Story Progression")
    void AdvanceStoryBeat(EStoryBeat NewBeat);


	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void PlayerDied(AAlanWakeCharacter* DeadPlayer);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Flow")
	void BP_HandleDeathSequence(AAlanWakeCharacter* DeadPlayer);

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void RespawnFromCheckpoint(AAlanWakeCharacter* PlayerToRespawn);

	UPROPERTY()
	TArray<ATakenBase*> ActiveEnemies;

	UFUNCTION()
	void RegisterActiveEnemy(ATakenBase* Enemy);

	UFUNCTION()
	void UnregisterEnemy(ATakenBase* Enemy);

	UFUNCTION()
	void DespawnEnemies();
};
