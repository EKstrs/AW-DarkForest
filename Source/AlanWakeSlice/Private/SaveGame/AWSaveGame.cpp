// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/AWSaveGame.h"

UAWSaveGame::UAWSaveGame()
{
	//Default values 
	PlayerLocation = FVector::ZeroVector;
	PlayerRotation = FRotator::ZeroRotator;
	RevolverMag = 0;
	RevolverReserve = 0;
	FlareGunAmmo = 0;
	FlareCount = 0;
	BatteryLevel = 1.0f;
	PlayerHealth = 100.0f;
	CurrentBeat = EStoryBeat::None;
}
