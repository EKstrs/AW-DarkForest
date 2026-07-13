// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AWGameMode.generated.h"

class ATakenBase;
/**
 * 
 */
UCLASS()
class ALANWAKESLICE_API AAWGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
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
