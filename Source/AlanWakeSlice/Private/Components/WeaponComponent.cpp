// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WeaponComponent.h"

#include "CollisionQueryParams.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"

UWeaponComponent::UWeaponComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

}

void UWeaponComponent::Initialize(UInventoryComponent* InInventoryComponent)
{
	InventoryComponent = InInventoryComponent;
}

void UWeaponComponent::EquipWeapon(EWeaponType NewWeapon)
{
	CurrentWeapon = NewWeapon;
}

bool UWeaponComponent::Fire(FVector MuzzleLocation, FVector AimDirection)
{
	if (!InventoryComponent || !InventoryComponent->TryConsumeAmmo(CurrentWeapon)) return false;

	if (CurrentWeapon == EWeaponType::Revolver)
	{
		FVector Start = MuzzleLocation;
		FVector End = Start + (AimDirection * 5000.f);
		FHitResult Hit;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner());

		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams))
		{
			if (Hit.GetActor() && Hit.GetActor()->ActorHasTag(TEXT("Enemy")))
			{
				float DamageToApply = BaseDamage;
				FString HitBone = Hit.BoneName.ToString().ToLower();
				
				if (HitBone.Contains(TEXT("head")))
				{
					DamageToApply *= 2.f;
				}
				else if (HitBone.Contains(TEXT("arm")) || HitBone.Contains(TEXT("leg")) || 
						 HitBone.Contains(TEXT("calf")) || HitBone.Contains(TEXT("thigh")))
				{
					DamageToApply *= 0.5f;
				}

				UGameplayStatics::ApplyPointDamage(Hit.GetActor(), DamageToApply, AimDirection, Hit, nullptr, GetOwner(), UDamageType::StaticClass());
			}
		}
		OnWeaponFiredDelegate.Broadcast(Hit.bBlockingHit, Hit.GetActor(), Hit.ImpactPoint, Hit.ImpactNormal);
	}
	return true;
}

void UWeaponComponent::Reload()
{
	if (InventoryComponent)
	{
		InventoryComponent->ReloadWeapon(CurrentWeapon, MaxRevolverMag);
	}
}


void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

