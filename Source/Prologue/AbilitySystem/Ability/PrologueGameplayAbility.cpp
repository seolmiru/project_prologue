// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Prologue/AbilitySystem/PrologueAbilitySystemComponent.h"
#include "Prologue/Component/Combat/PawnCombatComponent.h"

UPrologueGameplayAbility::UPrologueGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPrologueGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (AbilityActivationPolicy == EPrologueAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

void UPrologueGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (AbilityActivationPolicy == EPrologueAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo)
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}

UPawnCombatComponent* UPrologueGameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
	return GetAvatarActorFromActorInfo()->FindComponentByClass<UPawnCombatComponent>();
}

UPrologueAbilitySystemComponent* UPrologueGameplayAbility::GetPrologueAbilitySystemComponentFromActorInfo() const
{
	return Cast<UPrologueAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}
