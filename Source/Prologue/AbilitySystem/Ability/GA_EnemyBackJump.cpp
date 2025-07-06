// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_EnemyBackJump.h"

#include "AbilitySystemComponent.h"

bool UGA_EnemyBackJump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                           const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return !ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Shared.Cooldown.Attack.Run")));
}

void UGA_EnemyBackJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, true))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
}
