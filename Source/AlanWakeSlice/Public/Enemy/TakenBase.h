// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/FlashlightAffected.h"
#include "TakenBase.generated.h"

class UDarknessShield;
class UNiagaraSystem;
class UNiagaraComponent;
class UMotionWarpingComponent;

UCLASS()
class ALANWAKESLICE_API ATakenBase : public ACharacter, public IFlashlightAffected
{
	GENERATED_BODY()

public:

	ATakenBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TakenBase")
	UDarknessShield* DarknessShieldComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TakenBase")
	UStaticMeshComponent* MeleeWeaponMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TakenBase")
	UMotionWarpingComponent* MotionWarpingComponent;
	

	UFUNCTION()
	void OnShieldValueChanged(float ShieldPercent);
	
	
	virtual void ReceiveFlashlightExposure(float ExposureValue, bool bIsFocusBeam, const FVector& HitLocation, const FVector& HitNormal) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnFocusFlinch();

	UPROPERTY(BlueprintReadOnly)
	bool bIsBeingFocused = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* FlashlightHitVFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    UNiagaraSystem* ShieldBreakVFX;


	// Hit Reaction State
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Reactions")
	FName ReactionBoneName;

	UPROPERTY(BlueprintReadOnly, Category = "Animation|Reactions")
	FVector ImpactDirection;

	UPROPERTY(BlueprintReadOnly, Category = "Animation|Reactions")
	float ReactionAlpha;

	bool bIsFlinching = false;

	FName ResolveReactionBone(FName HitBone);

	void ProcessHit(FName HitBone, FVector HitDirection, FVector HitLocation);


	UFUNCTION(BlueprintCallable, Category = "AI System")
	void TriggerDespawn();

	UFUNCTION(BlueprintImplementableEvent, Category = "Attack")
	void BP_Attack();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackDamage = 15.f;

	UPROPERTY(BlueprintReadWrite, Category = "Attack")
	bool bIsAttacking = false;

	UPROPERTY(BlueprintReadWrite, Category = "Attack")
	bool bMeleeTraceActive = false;

	void PerformMeleeAttack();

	void StartMeleeTrace();
	void PerformMeleeTrace();
	void EndMeleeTrace();
	
	TArray<AActor*> HitActorsThisSwing;
	FVector LastTraceStart;
	FVector LastTraceEnd;

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type reason) override;
	virtual void Tick(float DeltaTime) override;

	
	USceneComponent* GetTraceData(FName& OutStartSocket, FName& OutEndSocket);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.f;
	float CurrentHealth = MaxHealth;

	UFUNCTION()
	void OnShieldDestroyed();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnShieldDestroyed();

	UFUNCTION(BlueprintImplementableEvent, Category = "Audio")
	void BP_UpdateShieldAudio(float ShieldPercentage);

	UFUNCTION(BlueprintImplementableEvent, Category = "Audio")
	void BP_StopShieldAudio();

	UFUNCTION(BlueprintImplementableEvent, Category = "VFX")
	void BP_OnDeath();

	UFUNCTION(BlueprintImplementableEvent, Category = "VFX")
	void BP_OnDespawn();

	UPROPERTY()
	UNiagaraComponent* ActiveFlashlightVFX;

	FTimerHandle FlashlightVFXStopTimer;

	UFUNCTION(BlueprintImplementableEvent, Category = "Flashlight FX")
	void BP_OnFlashlightHit(FVector HitLocation, float ExposureValue, bool bIsFocusBeam);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Flashlight FX")
	void BP_OnFlashlightStopped();
	
	void StopFlashlightVFX();


private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TakenBase", meta = (AllowPrivateAccess = "true"))
	float BaseWalkSpeed = 500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TakenBase",  meta = (AllowPrivateAccess = "true"))
	float FocusedWalkSpeed = 0.f;
	bool bHasFlinched = false;

	FTimerHandle SpeedResetTimer;

	void ResetSpeed();

	UPROPERTY(EditDefaultsOnly, Category = "VFX", meta = (AllowPrivateAccess = "true"))
	float ShieldBurnVFXInterval = 0.25f;

	float LastShieldBurnVFXTime = -1.f;
};




