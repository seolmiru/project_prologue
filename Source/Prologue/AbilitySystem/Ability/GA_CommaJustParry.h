// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_CommaJustParry.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_CommaJustParry : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_CommaJustParry();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnComplete();

	UFUNCTION()
	void OnCurveTick(float Alpha);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCurveFloat> Curve;
};
