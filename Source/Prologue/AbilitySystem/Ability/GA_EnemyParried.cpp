// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_EnemyParried.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Prologue/Character/Enemy/PrologueEnemyCharacter.h"

void UGA_EnemyParried::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APrologueEnemyCharacter* Enemy = Cast<APrologueEnemyCharacter>(ActorInfo->AvatarActor.Get());

	Enemy->GetCharacterMovement()->SetMovementMode(MOVE_None);
}

void UGA_EnemyParried::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	APrologueEnemyCharacter* Enemy = Cast<APrologueEnemyCharacter>(ActorInfo->AvatarActor.Get());

	Enemy->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
