// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_OverClock.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_OverClock : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_OverClock();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnOverClockFinished();

	UPROPERTY(EditAnywhere)
	float OverClockDuration = 3.0f;

	UPROPERTY(EditAnywhere)
	float GlobalTimeScale = 0.2f;

	FTimerHandle OverClockTimerHandle;
};
