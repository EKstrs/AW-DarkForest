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
#include "Components/SkeletalMeshComponent.h"
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

	//Default states
	CurrentEquipState = EEquipState::Unarmed;
	bHasThrowable = false;
}


void AAlanWakeCharacter::BeginPlay()
{
	Super::BeginPlay();
}




void AAlanWakeCharacter::EquipItem(EEquipState NewState)
{
	if (CurrentEquipState == NewState) return;

	if (NewState == EEquipState::Throwable && !bHasThrowable) return;

	PendingEquipState = NewState;
	if (EquipMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(EquipMontage);
	}
	else
	{
		FinalizeEquip();
	}
}

void AAlanWakeCharacter::FinalizeEquip()
{
	CurrentEquipState = PendingEquipState;
	
	OnEquipStateChangedDelegate.Broadcast(CurrentEquipState);
}

void AAlanWakeCharacter::ReleaseThrowable()
{
	if (!ThrowableClass) return;

	FVector SpawnLocation = GetActorLocation() + (GetActorForwardVector() * 80.0f);
	SpawnLocation.Z += 60.0f;

	FRotator ThrowRotation = GetActorRotation();
	if (FollowCamera)
	{
		ThrowRotation = FollowCamera->GetComponentRotation();
		ThrowRotation.Pitch += 30.0f;
	}

	//Spawn parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(ThrowableClass, SpawnLocation, ThrowRotation, SpawnParams);

	bHasThrowable = false;
	ThrowableClass = nullptr;
	bIsAiming = false;
	EquipItem(EEquipState::Unarmed);
}

void AAlanWakeCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AAlanWakeCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AAlanWakeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAlanWakeCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAlanWakeCharacter::Look);

		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this,
		                                   &AAlanWakeCharacter::ToggleCrouch);
		
		//Throw
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Started, this,
		                                   &AAlanWakeCharacter::ThrowItem);

		//Aim
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &AAlanWakeCharacter::StartAim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AAlanWakeCharacter::StopAim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Canceled, this, &AAlanWakeCharacter::StopAim);

		//Fire weapon
		EnhancedInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Started, this,
		                                   &AAlanWakeCharacter::FireWeapon);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AAlanWakeCharacter::Reload);
	
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

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
	if (CurrentEquipState != EEquipState::Throwable || !bHasThrowable || !ThrowMontage || !bIsAiming) return;

	PlayAnimMontage(ThrowMontage);
}

void AAlanWakeCharacter::StartAim(const FInputActionValue& value)
{
	if (!bCanAim)
	{
		bIsAiming = false;
		return;
	}
	bIsAiming = true;
}

void AAlanWakeCharacter::StopAim(const FInputActionValue& value)
{
	bIsAiming = false;
}

void AAlanWakeCharacter::FireWeapon(const FInputActionValue& value)
{
	if (CurrentEquipState != EEquipState::Pistol || CurrentAmmo <= 0 || !bIsAiming) return;

	if (bCanFire)
	{
		CurrentAmmo--;
		//Set back to true in the blueprint event
		bCanFire = false;
		if (FollowCamera)
		{
			FVector Start = FollowCamera->GetComponentLocation();
			FVector End = FollowCamera->GetComponentLocation() + (FollowCamera->GetForwardVector() * 5000.f);
			FHitResult Hit;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);
			bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams);
			float DamageToApply = BaseDamage;
			if (bHit)
			{
				if (Hit.GetActor() && Hit.GetActor()->ActorHasTag(TEXT("Enemy")))
				{
					FString HitBone = Hit.BoneName.ToString().ToLower();

					if (HitBone.Contains(TEXT("head")))
					{
						DamageToApply *= 10.f;
					}
					else if (HitBone.Contains(TEXT("arm")) || HitBone.Contains(TEXT("leg")) || HitBone.
						Contains(TEXT("calf")) || HitBone.Contains(TEXT("thigh")))
					{
						DamageToApply *= 0.5f;
					}
					if (APlayerController* PC = Cast<APlayerController>(GetLocalViewingPlayerController()))
					{
						UGameplayStatics::ApplyDamage(Hit.GetActor(), DamageToApply, GetController(), this,
													  UDamageType::StaticClass());
					}
				}
			}
			BP_OnWeaponFired(bHit, Hit.GetActor(), Hit.ImpactPoint, Hit.ImpactNormal, Hit.BoneName);
		}
	}
}

void AAlanWakeCharacter::Reload(const FInputActionValue& value)
{

	const int32 MaxMagAmmo = 7;
	if (ReserveAmmo <= 0 || CurrentAmmo >= MaxMagAmmo) return;


	PlayAnimMontage(ReloadMontage);
	bCanAim = false;
	bIsAiming = false;
	
	int32 AmmoNeeded = MaxMagAmmo - CurrentAmmo;
	int32 AmmoToMove = FMath::Min(AmmoNeeded, ReserveAmmo);
	
	CurrentAmmo += AmmoToMove;
	ReserveAmmo -= AmmoToMove;
}
