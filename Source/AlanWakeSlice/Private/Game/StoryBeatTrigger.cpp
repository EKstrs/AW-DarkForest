// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/StoryBeatTrigger.h"

#include "Character/AlanWakeCharacter.h"
#include "Components/BillboardComponent.h"
#include "Engine/World.h"
#include "Game/AWGameMode.h"

AStoryBeatTrigger::AStoryBeatTrigger()
{
}

void AStoryBeatTrigger::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);


	if (Cast<AAlanWakeCharacter>(OtherActor))
	{
		if (AAWGameMode* GM = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode()))
		{
			if (GM->CurrentBeat == RequiredPreviousBeat)
			{
				GM->AdvanceStoryBeat(BeatToTrigger);
			}
		}
		
	}
}
