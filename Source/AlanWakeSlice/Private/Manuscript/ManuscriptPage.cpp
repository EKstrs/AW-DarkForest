// Fill out your copyright notice in the Description page of Project Settings.


#include "Manuscript/ManuscriptPage.h"

#include "Character/AlanWakeCharacter.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AManuscriptPage::AManuscriptPage()
{
	PrimaryActorTick.bCanEverTick = false;

	PaperMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PaperMesh"));
	RootComponent = PaperMesh;
	PaperMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetupAttachment(PaperMesh);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	Sphere->SetupAttachment(PaperMesh);
	Sphere->SetSphereRadius(150.f);
	Sphere->SetCollisionProfileName(TEXT("Trigger"));
	

}


void AManuscriptPage::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AManuscriptPage::OnOverlapBegin);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AManuscriptPage::OnOverlapEnd);
	
}

bool AManuscriptPage::TryInteract(class AAlanWakeCharacter* Player)
{
	bIsBeingRead = !bIsBeingRead;
	BP_OnToggleReadUI(bIsBeingRead);

	return true;
}

FString AManuscriptPage::GetInteractableName()
{
	return ItemName;
}

bool AManuscriptPage::CanInteract(class AAlanWakeCharacter* Player)
{
	return true;
}


void AManuscriptPage::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AAlanWakeCharacter* Player = Cast<AAlanWakeCharacter>(OtherActor))
	{
		Player->RegisterInteractable(this);
	}
}

void AManuscriptPage::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AAlanWakeCharacter* Player = Cast<AAlanWakeCharacter>(OtherActor))
	{
		Player->UnregisterInteractable(this);
	}
}
