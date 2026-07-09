// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "PickupBase.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class ALANWAKESLICE_API APickupBase : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	
	APickupBase();
	
	virtual FString GetInteractableName() override;
	
	virtual bool TryInteract(class AAlanWakeCharacter* Player) override;


	virtual bool CanInteract(class AAlanWakeCharacter* Player) override;


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup Setup")
	FString ItemName = "Unknown Item";
	
	virtual void BeginPlay() override;
	
	virtual bool ProcessPickup(class AAlanWakeCharacter* Player);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* OverlapSphere;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void BP_OnPickedUp();
};
