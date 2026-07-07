// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SavePointActor.generated.h"


class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class ALANWAKESLICE_API ASavePointActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ASavePointActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* OverlapSphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SaveSystem")
	FString SaveSlotName;

	// Prevents the checkpoint from firing multiple times
	UPROPERTY(BlueprintReadWrite, Category = "SaveSystem")
	bool bHasTriggered;

	// The function that fires when something walks into the sphere
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Implement this in Blueprint to pop up the "Checkpoint Reached" text and play a sound!
	UFUNCTION(BlueprintImplementableEvent, Category = "SaveSystem")
	void BP_OnCheckpointReached();
};
