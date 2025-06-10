// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_EnemyDetect.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Prologue/Character/Enemy/PrologueEnemyCharacter.h"

void UGA_EnemyDetect::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APrologueEnemyCharacter* Enemy = CastChecked<APrologueEnemyCharacter>(ActorInfo->AvatarActor.Get());

	Enemy->GetCharacterMovement()->SetMovementMode(MOVE_None);
	
	if (APawn* AvatarPawn = Cast<APawn>(CurrentActorInfo->AvatarActor.Get()))
	{
		if (AAIController* AIController = Cast<AAIController>(AvatarPawn->GetController()))
		{
			if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
			{
				BlackboardComponent->SetValueAsBool(FName("IsPlayed"), true);
			}
		}
	}
}

void UGA_EnemyDetect::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	APrologueEnemyCharacter* Enemy = CastChecked<APrologueEnemyCharacter>(ActorInfo->AvatarActor.Get());

	Enemy->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
