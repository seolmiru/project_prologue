// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAsset_CommaStartUpData.h"
#include "../AbilitySystem/Ability/PrologueGameplayAbility.h"
#include "../AbilitySystem/PrologueAbilitySystemComponent.h"

void UDataAsset_CommaStartUpData::GiveToAbilitySystemComponent(UPrologueAbilitySystemComponent* InASCToGive,
	int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

	for (const FPrologueCommaAbilitySet& AbilitySet : CommaStartUpAbilitySets)
	{
		if (!AbilitySet.IsValid()) continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);

		InASCToGive->GiveAbility(AbilitySpec);
	}
}
