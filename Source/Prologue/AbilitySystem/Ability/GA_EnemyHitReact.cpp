// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_EnemyHitReact.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Prologue/AbilitySystem/Attribute/PrologueAttributeSet.h"
#include "Prologue/Character/Enemy/PrologueEnemyCharacter.h"

void UGA_EnemyHitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APrologueEnemyCharacter* Enemy = CastChecked<APrologueEnemyCharacter>(ActorInfo->AvatarActor.Get());
	
	Enemy->GetCharacterMovement()->SetMovementMode(MOVE_None);
}

void UGA_EnemyHitReact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	// 강인도 초기화
	ASC->SetNumericAttributeBase(UPrologueAttributeSet::GetCurrentToughnessAttribute(), 100.0f);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	APrologueEnemyCharacter* Enemy = CastChecked<APrologueEnemyCharacter>(ActorInfo->AvatarActor.Get());
	
	Enemy->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
