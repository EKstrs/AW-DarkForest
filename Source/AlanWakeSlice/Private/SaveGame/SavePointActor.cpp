// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/SavePointActor.h"

#include "Character/AlanWakeCharacter.h"
#include "Components/FlashlightComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Game/AWGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/AWSaveGame.h"

ASavePointActor::ASavePointActor()
{

	PrimaryActorTick.bCanEverTick = false;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(RootComponent);
	
	// Make the sphere large enough for a safe zone
	OverlapSphere->SetSphereRadius(300.0f);
	OverlapSphere->SetGenerateOverlapEvents(true);

	SaveSlotName = TEXT("Slot1");
	bHasTriggered = false;

}

void ASavePointActor::BeginPlay()
{
	Super::BeginPlay();
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ASavePointActor::OnOverlapBegin);
	
}

void ASavePointActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasTriggered) return;
	
	if (AAlanWakeCharacter* Player = Cast<AAlanWakeCharacter>(OtherActor))
	{
		bHasTriggered = true;
		
		if (AAWGameMode* GM = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->DespawnEnemies();
			UAWSaveGame* SaveGameInstance = Cast<UAWSaveGame>(UGameplayStatics::CreateSaveGameObject(UAWSaveGame::StaticClass()));
			
			if (SaveGameInstance)
			{
				// Populate the data from the player
				SaveGameInstance->PlayerLocation = Player->GetActorLocation();
				SaveGameInstance->PlayerRotation = Player->GetActorRotation();
				SaveGameInstance->BatteryLevel = Player->FlashlightComp->GetBatteryLevel();
				SaveGameInstance->RevolverMag = Player->InventoryComp->GetRevolverMag();
				SaveGameInstance->RevolverReserve =  Player->InventoryComp->GetRevolverReserve();
				SaveGameInstance->FlareGunAmmo = Player->InventoryComp->GetFlareGunAmmo();
				SaveGameInstance->FlareCount = Player->InventoryComp->GetFlareCount();
				SaveGameInstance->PlayerHealth = Player->GetHealth();
				SaveGameInstance->CurrentBeat = GM->CurrentBeat;
				
				UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, 0);
				
				BP_OnCheckpointReached();
			}
		}	
	}
}


