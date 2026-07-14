// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "GeneratorPuzzle.generated.h"

class USphereComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPowerRestoredSignature);

UCLASS()
class ALANWAKESLICE_API AGeneratorPuzzle : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AGeneratorPuzzle();
	
	UFUNCTION(BlueprintCallable, Category = "GeneratorPuzzle")
	void AttemptPull(bool bWasSweetSpotHit);

	virtual FString GetInteractableName() override;
	
	virtual bool TryInteract(class AAlanWakeCharacter* Player) override;


	virtual bool CanInteract(class AAlanWakeCharacter* Player) override;
	
protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|State")
	int32 TargetSuccesses = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Generator|State")
	int32 CurrentSuccesses = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generator|State")
	bool bIsPoweredOn = false;

	UPROPERTY(BlueprintAssignable, Category = "Generator|State")
	FOnPowerRestoredSignature OnPowerRestoredDelegate;

	UFUNCTION(BlueprintImplementableEvent, Category = "Generator|UI")
	void BP_ShowQTEWidget();

	UFUNCTION(BlueprintImplementableEvent, Category = "Generator|UI")
	void BP_HideQTEWidget();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* OverlapSphere;


	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:

	FString Name = "Generator";
};
