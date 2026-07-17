// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "StoryBeatTrigger.generated.h"

enum class EStoryBeat : uint8;
/**
 * 
 */
UCLASS()
class ALANWAKESLICE_API AStoryBeatTrigger : public ATriggerBox
{
	GENERATED_BODY()
	
	
	
public:

	AStoryBeatTrigger();


protected:

	UPROPERTY(EditAnywhere, Category = "Story Progression")
	EStoryBeat BeatToTrigger;

	UPROPERTY(EditAnywhere, Category = "Story Progression")
	EStoryBeat RequiredPreviousBeat;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
