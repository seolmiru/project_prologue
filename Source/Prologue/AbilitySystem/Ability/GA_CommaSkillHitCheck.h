// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_CommaSkillHitCheck.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_CommaSkillHitCheck : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_CommaSkillHitCheck();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UPROPERTY(EditAnywhere, Category = "Effect")
	TSubclassOf<UGameplayEffect> AttackDamageEffect;

	UPROPERTY(EditAnywhere, Category = "Trace")
	TSubclassOf<class ATA_Trace> TargetActorClass;
};
