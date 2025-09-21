// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AttackHitCheck.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_AttackHitCheck : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_AttackHitCheck();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UPROPERTY(EditAnywhere, Category = "Effect")
	TSubclassOf<UGameplayEffect> AttackDamageEffect;

	UPROPERTY(EditAnywhere, Category = "Effect")
	TSubclassOf<UGameplayEffect> ToughnessDamageEffect;
	
	UPROPERTY(EditAnywhere, Category = "Trace")
	TSubclassOf<class ATA_Trace> TargetActorClass;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHitDetected = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHitNormalTarget = false;
};