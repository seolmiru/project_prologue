// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CommaHitReact.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/Character/Player/Comma.h"

UGA_CommaHitReact::UGA_CommaHitReact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_CommaHitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AComma* Comma = CastChecked<AComma>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	Comma->GetCharacterMovement()->SetMovementMode(MOVE_None);

	if (ASC->HasMatchingGameplayTag(PrologueGameplayTags::Comma_Weapon_Sword))
	{
		bIsSword = true;
	}
	else
	{
		bIsSword = false;
	}
}

void UGA_CommaHitReact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	AComma* Comma = CastChecked<AComma>(ActorInfo->AvatarActor.Get());

	Comma->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
