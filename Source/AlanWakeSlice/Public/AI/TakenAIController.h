// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TakenAIController.generated.h"

/**
 * 
 */
UCLASS()
class ALANWAKESLICE_API ATakenAIController : public AAIController
{
	GENERATED_BODY()


public:

	ATakenAIController();
	virtual void Tick(float DeltaTime) override;

protected:

	virtual void OnPossess(APawn* InPawn) override;


private:	
	UPROPERTY()
	TObjectPtr<AActor> TargetPlayer;

	FVector SpawnLocation;
	
	UPROPERTY(EditDefaultsOnly, Category = "Taken AI")
	float MaxChaseDistance = 4000.0f; 

	UPROPERTY(EditDefaultsOnly, Category = "Taken AI")
	float AttackAcceptanceRadius = 150.0f;
	
	
	
	
};
