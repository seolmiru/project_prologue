 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_OverClock.generated.h"

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

	UPROPERTY(EditAnywhere)
	float OverClockDuration = 3.0f;

	UPROPERTY(EditAnywhere)
	float TimeScale = 0.2f;

	FTimerHandle OverClockTimerHandle;

protected:
	TArray<APrologueEnemyCharacter*> AffectedEnemies;

	TArray<ABazierProjectile*> AffectedProjectiles;

	void ApplySlowToEnemies();
	void RestoreEnemyTime();
};
