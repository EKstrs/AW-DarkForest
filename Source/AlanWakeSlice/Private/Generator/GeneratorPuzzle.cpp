// Fill out your copyright notice in the Description page of Project Settings.


#include "Generator/GeneratorPuzzle.h"

#include "Character/AlanWakeCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AGeneratorPuzzle::AGeneratorPuzzle()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetSphereRadius(150.f); 
	OverlapSphere->SetCollisionProfileName(TEXT("Trigger"));

}
void AGeneratorPuzzle::BeginPlay()
{
	Super::BeginPlay();

	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AGeneratorPuzzle::OnOverlapBegin);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &AGeneratorPuzzle::OnOverlapEnd);
}

void AGeneratorPuzzle::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AAlanWakeCharacter* Player = Cast<AAlanWakeCharacter>(OtherActor))
	{
		Player->RegisterInteractable(this);
	}
}

void AGeneratorPuzzle::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AAlanWakeCharacter* Player = Cast<AAlanWakeCharacter>(OtherActor))
	{
		Player->UnregisterInteractable(this);
	}
}


void AGeneratorPuzzle::AttemptPull(bool bWasSweetSpotHit)
{
	if (bIsPoweredOn) return;

	if (bWasSweetSpotHit)
	{
		CurrentSuccesses++;

		if (CurrentSuccesses >= TargetSuccesses)
		{
			bIsPoweredOn = true;
			BP_HideQTEWidget();
			OnPowerRestoredDelegate.Broadcast();
		}
	}
	else
	{
		CurrentSuccesses = 0;
	}
}

FString AGeneratorPuzzle::GetInteractableName()
{
	return Name;
}

bool AGeneratorPuzzle::TryInteract(class AAlanWakeCharacter* Player)
{
	if (bIsPoweredOn) return false;
	BP_ShowQTEWidget();
	return true;
}

bool AGeneratorPuzzle::CanInteract(class AAlanWakeCharacter* Player)
{
	return !bIsPoweredOn;
}

