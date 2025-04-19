// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PrologueGameplayAbility.generated.h"

class UPrologueAbilitySystemComponent;
class UPawnCombatComponent;

UENUM(BlueprintType)
enum class EPrologueAbilityActivationPolicy : uint8
{
	OnTriggered,
	OnGiven
};

/**
 * 
 */
UCLASS()
class PROLOGUE_API UPrologueGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	EPrologueAbilityActivationPolicy AbilityActivationPolicy = EPrologueAbilityActivationPolicy::OnTriggered;

	UFUNCTION(BlueprintPure, Category = "Prologue|Ability")
	UPawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Prologue|Ability")
	UPrologueAbilitySystemComponent* GetPrologueAbilitySystemComponentFromActorInfo() const;
};  
