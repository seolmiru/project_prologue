// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_EnemyAttack.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/Character/Enemy/PrologueEnemyCharacter.h"

UGA_EnemyAttack::UGA_EnemyAttack()
{
	
}

bool UGA_EnemyAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;	
	}

	return !ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Shared.Cooldown.Attack")));
}

void UGA_EnemyAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (ParryTimingEffectClass)
	{
		FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(ParryTimingEffectClass, 0.f, EffectContextHandle);
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
	
	APrologueEnemyCharacter* Enemy = CastChecked<APrologueEnemyCharacter>(ActorInfo->AvatarActor.Get());

	Enemy->GetCharacterMovement()->SetMovementMode(MOVE_None);
}

void UGA_EnemyAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	APrologueEnemyCharacter* Enemy = CastChecked<APrologueEnemyCharacter>(ActorInfo->AvatarActor.Get());

	Enemy->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
