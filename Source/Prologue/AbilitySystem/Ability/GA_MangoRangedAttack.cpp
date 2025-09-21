// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_MangoRangedAttack.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Prologue/Character/Enemy/PrologueEnemyCharacter.h"

UGA_MangoRangedAttack::UGA_MangoRangedAttack()
{
}

void UGA_MangoRangedAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, true))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	APrologueEnemyCharacter* Enemy = CastChecked<APrologueEnemyCharacter>(ActorInfo->AvatarActor.Get());

	Enemy->GetCharacterMovement()->SetMovementMode(MOVE_None);
}

void UGA_MangoRangedAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	APrologueEnemyCharacter* Enemy = CastChecked<APrologueEnemyCharacter>(ActorInfo->AvatarActor.Get());

	Enemy->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
