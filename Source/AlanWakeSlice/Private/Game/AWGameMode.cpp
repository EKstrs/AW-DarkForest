// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AWGameMode.h"

#include "Character/AlanWakeCharacter.h"

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
