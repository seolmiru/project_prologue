// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Chronos_Summon_ClockHand.h"

#include "AbilitySystemComponent.h"

void UGA_Chronos_Summon_ClockHand::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                   const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                   const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Invincible Effect
	FGameplayEffectContextHandle InvincibleEffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	InvincibleEffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle InvincibleEffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(InvincibleEffect, 0.f, InvincibleEffectContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*InvincibleEffectSpecHandle.Data.Get());
}
