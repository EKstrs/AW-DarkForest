// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/PickupBase.h"

#include "Character/AlanWakeCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

APickupBase::APickupBase()
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
void APickupBase::BeginPlay()
{
	Super::BeginPlay();

	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &APickupBase::OnOverlapBegin);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &APickupBase::OnOverlapEnd);
	
}

FString APickupBase::GetInteractableName()
{
	return ItemName;
}

bool APickupBase::TryInteract(class AAlanWakeCharacter* Player)
{
	if (ProcessPickup(Player))
	{
		BP_OnPickedUp();
		Destroy();
		return true;
	}
	return false;
}

bool APickupBase::CanInteract(class AAlanWakeCharacter* Player)
{
	return false;
}

bool APickupBase::ProcessPickup(class AAlanWakeCharacter* Player)
{
	return false;
}

void APickupBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AAlanWakeCharacter* Player = Cast<AAlanWakeCharacter>(OtherActor))
	{
		Player->RegisterInteractable(this);
	}
}

void APickupBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (AAlanWakeCharacter* Player = Cast<AAlanWakeCharacter>(OtherActor))
	{
		Player->UnregisterInteractable(this);
	}
}





