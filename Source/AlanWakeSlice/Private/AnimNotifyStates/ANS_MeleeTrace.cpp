// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyStates/ANS_MeleeTrace.h"

#include "Components/SkeletalMeshComponent.h"
#include "Enemy/TakenBase.h"

void UANS_MeleeTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                  const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (ATakenBase* Enemy = Cast<ATakenBase>(MeshComp->GetOwner()))
	{
		Enemy->StartMeleeTrace();
	}
}

void UANS_MeleeTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (ATakenBase* Enemy = Cast<ATakenBase>(MeshComp->GetOwner()))
	{
		Enemy->PerformMeleeTrace();
	}
}

void UANS_MeleeTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (ATakenBase* Enemy = Cast<ATakenBase>(MeshComp->GetOwner()))
	{
		Enemy->EndMeleeTrace();
	}
}
