// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_RemoveSwitchAttackTag.h"

#include "AbilitySystemComponent.h"

void UGA_RemoveSwitchAttackTag::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	FGameplayTagContainer SwitchAttackTag;
	SwitchAttackTag.AddTag(FGameplayTag::RequestGameplayTag("Comma.State.SwitchAttack"));
	
	ASC->CancelAbilities(&SwitchAttackTag);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_RemoveSwitchAttackTag::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
}

void UGA_RemoveSwitchAttackTag::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_RemoveSwitchAttackTag::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
