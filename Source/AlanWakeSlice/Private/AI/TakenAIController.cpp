// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/TakenAIController.h"

#include "Kismet/GameplayStatics.h"

ATakenAIController::ATakenAIController()
{
}

void ATakenAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!TargetPlayer)
	{
		TargetPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
		if (!TargetPlayer) return;
	}
	if (!GetPawn()) return;

	if (FVector::Dist(SpawnLocation, GetPawn()->GetActorLocation()) > MaxChaseDistance)
	{
		GetPawn()->Destroy();
		return;
	}

	MoveToActor(TargetPlayer, AttackAcceptanceRadius);

	if (FVector::Dist(GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation()) <= AttackAcceptanceRadius)
	{
		//TODO Attack player
	}
}

void ATakenAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	SpawnLocation = InPawn->GetActorLocation();
	TargetPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
}
