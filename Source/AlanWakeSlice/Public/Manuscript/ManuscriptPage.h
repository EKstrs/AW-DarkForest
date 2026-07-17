// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "ManuscriptPage.generated.h"

class USphereComponent;
class UPointLightComponent;

UCLASS()
class ALANWAKESLICE_API AManuscriptPage : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AManuscriptPage();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PaperMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPointLightComponent* PointLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* Sphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString ItemName = TEXT("Manuscript page");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manuscript Content", meta = (MultiLine = "true"))
	FText ManuscriptText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsBeingRead = false;

	virtual bool TryInteract(class AAlanWakeCharacter* Player) override;
	virtual FString GetInteractableName() override;
	virtual bool CanInteract(class AAlanWakeCharacter* Player) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnToggleReadUI(bool ShowUI);


	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
