// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_CommaHitStop.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_CommaHitStop : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_CommaHitStop();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitStop")
	float HitStopDuration = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitStop")
	float HitStopTimeScale = 0.1f;
	
private:
	void HitStop();
	void EndHitStop();

	FTimerHandle HitStopTimerHandle;
	
	bool bHitStopApplied = false;
};
