// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CommaHeal.h"

#include "AbilitySystemComponent.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/AbilitySystem/PrologueAttributeSet.h"

UGA_CommaHeal::UGA_CommaHeal()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_CommaHeal::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	const UPrologueAttributeSet* AttributeSet = ASC->GetSet<UPrologueAttributeSet>();

	if (AttributeSet->GetCurrentHealth() >= AttributeSet->GetMaxHealth())
	{
		return false;
	}

	return true;
}


void UGA_CommaHeal::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 힐 이펙트 적용
	FGameplayEffectContextHandle HealEffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	HealEffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle HealEffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(HealAmountEffect, 0.f, HealEffectContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*HealEffectSpecHandle.Data.Get());

	// 힐 VFX 출력
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_Heal);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_CommaHeal::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
