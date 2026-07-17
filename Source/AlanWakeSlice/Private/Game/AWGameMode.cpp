// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AWGameMode.h"

#include "Character/AlanWakeCharacter.h"
#include "Enemy/TakenBase.h"

void AAWGameMode::AdvanceStoryBeat(EStoryBeat NewBeat)
{
	if ((uint8)NewBeat <= (uint8)CurrentBeat)
	{
		return;
	}
	CurrentBeat = NewBeat;
	OnStoryBeatChanged.Broadcast(CurrentBeat);
}

void AAWGameMode::PlayerDied(AAlanWakeCharacter* DeadPlayer)
{
	if (DeadPlayer)
	{
		BP_HandleDeathSequence(DeadPlayer);
	}
}

void AAWGameMode::RespawnFromCheckpoint(AAlanWakeCharacter* PlayerToRespawn)
{
	if (PlayerToRespawn)
	{
		PlayerToRespawn->LoadPlayerState();
	}
}

void AAWGameMode::RegisterActiveEnemy(ATakenBase* Enemy)
{
	ActiveEnemies.AddUnique(Enemy);
}

void AAWGameMode::UnregisterEnemy(ATakenBase* Enemy)
{
	ActiveEnemies.Remove(Enemy);
}

void AAWGameMode::DespawnEnemies()
{
	TArray<ATakenBase*> Enemies =  ActiveEnemies;
	ActiveEnemies.Empty();

	for (ATakenBase* Enemy: Enemies)
	{
		if (IsValid(Enemy))
		{
			Enemy->TriggerDespawn();
		}
	}
}
