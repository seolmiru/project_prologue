 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_OverClock.generated.h"

 class APrologueProjectileBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeScale, float, NewScale);

class ABazierProjectile;
class APrologueEnemyCharacter;
/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_OverClock : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_OverClock();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	static bool bIsOverClockActive;
	static float OverClockTimeScale;
	static FOnTimeScale OnTimeScale;

protected:
	UFUNCTION()
	void OnOverClockFinished();

	UFUNCTION()
	void CheckActorsInArea();

	UPROPERTY(EditAnywhere, Category = "OverClock")
	float OverClockDuration = 3.0f;

	UPROPERTY(EditAnywhere, Category = "OverClock")
	float TimeScale = 0.2f;

	UPROPERTY(EditAnywhere, Category = "OverClock")
	float Radius = 500.f;

	UPROPERTY(EditAnywhere, Category = "OverClock")
	float HalfHeight = 500.f;	

	UPROPERTY(EditAnywhere, Category = "OverClock")
	float CheckInterval = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShowDebug = true;
	
	FTimerHandle OverClockTimerHandle;

	FTimerHandle CheckAreaTimerHandle;

protected:
	TSet<TWeakObjectPtr<AActor>> AffectedActors;

	FVector CenterLocation;
	
	void RestoreEnemyTime();

	TMap<TWeakObjectPtr<AActor>, TArray<UMaterialInstanceDynamic*>> AffectedActorMaterial;
};
