//Written by Eetu Karhinen
#include "Character/AlanWakeCharacter.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Animation/AnimInstance.h"
#include "Components/FlashlightComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/WeaponComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/DamageType.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AAlanWakeCharacter

AAlanWakeCharacter::AAlanWakeCharacter()
{
	//Allow the character to use tick for debug sphere
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; 
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; 

	
	//Create mesh for item attached to hand
	HandItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HandItemMesh"));
	HandItemMesh->SetupAttachment(GetMesh(), FName("HandSocket"));
	HandItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HandItemMesh->SetVisibility(false);


	//Custom Components
	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	WeaponComp = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
	FlashlightComp = CreateDefaultSubobject<UFlashlightComponent>(TEXT("FlashlightComponent"));

	
	FlashlightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlashlightMesh"));
	FlashlightMesh->SetupAttachment(GetMesh(), FName("FlashlightSocket")); 
	FlashlightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	FlashlightInner = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashlightInner"));
	FlashlightInner->SetupAttachment(FlashlightMesh);

	FlashlightOuter = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashlightOuter"));
	FlashlightOuter->SetupAttachment(FlashlightMesh);
	

}


void AAlanWakeCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (WeaponComp && InventoryComp)
	{
		WeaponComp->Initialize(InventoryComp);
	}
	
	if (FlashlightComp && FlashlightInner && FlashlightOuter)
	{
		FlashlightComp->SetupLights(FlashlightInner, FlashlightOuter);
		FlashlightComp->SetCamera(FollowCamera);
	}
}

void AAlanWakeCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

////////////////////////////////////////
///Input actions

float AAlanWakeCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float Damage =  Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHealth = FMath::Clamp<float>(CurrentHealth - Damage, 0.f, MaxHealth);

	OnHealthChangedDelegate.Broadcast(CurrentHealth);

	if (CurrentHealth <= 0 && !bIsDead)
	{
		OnDeath();
	}
	return Damage;
}

void AAlanWakeCharacter::OnDeath()
{
	bIsDead = true;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}
}

void AAlanWakeCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AAlanWakeCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AAlanWakeCharacter::SprintStart(const FInputActionValue& Value)
{
	BP_OnSprintStarted();
}

void AAlanWakeCharacter::SprintEnd(const FInputActionValue& Value)
{
	BP_OnSprintStopped();
}

void AAlanWakeCharacter::ToggleCrouch(const FInputActionValue& Value)
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AAlanWakeCharacter::ThrowItem(const FInputActionValue& value)
{
	
}
void AAlanWakeCharacter::EquipWeapon(EWeaponType NewWeaponType)
{
	if (WeaponComp)
	{
		WeaponComp->EquipWeapon(NewWeaponType);
		BP_UpdateWeaponVisuals();
	}
}


void AAlanWakeCharacter::StartAim(const FInputActionValue& value)
{
	if (!bCanAim)
	{
		bIsAiming = false;
		return;
	}
	bIsAiming = true;
    
	if (FlashlightComp)
	{
		FlashlightComp->SetFocusMode(true);
	}
	
	BP_OnAimStarted();
	
}

void AAlanWakeCharacter::StopAim(const FInputActionValue& value)
{
	bIsAiming = false;
	if (FlashlightComp)
	{
		FlashlightComp->SetFocusMode(false);
	}
	BP_OnAimStopped();
}
void AAlanWakeCharacter::FireWeapon(const FInputActionValue& value)
{
	if (bIsAiming && FollowCamera && WeaponComp)
	{
		if (!WeaponComp->Fire(FollowCamera->GetComponentLocation(), FollowCamera->GetForwardVector()))
		{
			BP_PlayDryFire();
		}
	}
}

void AAlanWakeCharacter::Reload(const FInputActionValue& value)
{
	if (WeaponComp)
	{
		WeaponComp->Reload();
		
		if (ReloadMontage)
		{
			PlayAnimMontage(ReloadMontage);
			bCanAim = false;
			bIsAiming = false;
		}
	}
}

void AAlanWakeCharacter::ToggleFlashlight(const FInputActionValue& value)
{
	if (FlashlightComp)
	{
		FlashlightComp->ToggleFlashlight();
	}
}
