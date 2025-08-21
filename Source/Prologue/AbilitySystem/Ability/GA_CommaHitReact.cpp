// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CommaHitReact.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/AbilitySystem/Attribute/PrologueAttributeSet.h"
#include "Prologue/Character/Player/Comma.h"

void UGA_CommaHitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AComma* Comma = CastChecked<AComma>(ActorInfo->AvatarActor.Get());
	
	Comma->GetCharacterMovement()->SetMovementMode(MOVE_None);
}

void UGA_CommaHitReact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	// 강인도 초기화
	ASC->SetNumericAttributeBase(UPrologueAttributeSet::GetCurrentToughnessAttribute(), 100.0f);
	
	AComma* Comma = CastChecked<AComma>(ActorInfo->AvatarActor.Get());

	Comma->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
