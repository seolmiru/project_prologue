// Fill out your copyright notice in the Description page of Project Settings.


#include "../AbilitySystem/PrologueAbilitySystemComponent.h"
#include "../AbilitySystem/Ability/PrologueGameplayAbility.h"

void UPrologueAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid())
	{
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag)) continue;

		TryActivateAbility(AbilitySpec.Handle);
	}
}

void UPrologueAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
}

void UPrologueAbilitySystemComponent::GrantCommaWeaponAbilities(
	const TArray<FPrologueCommaAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel)
{
	if (InDefaultWeaponAbilities.IsEmpty())
	{
		return;
	}

	for (const FPrologueCommaAbilitySet& AbilitySet : InDefaultWeaponAbilities)
	{
		if (!AbilitySet.IsValid()) continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);
		
		GiveAbility(AbilitySpec);
	}
}

