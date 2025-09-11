// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_EventTrigger.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Prologue/Prologue.h"

UAnimNotify_EventTrigger::UAnimNotify_EventTrigger()
{
}

void UAnimNotify_EventTrigger::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		if (AActor* OwnerActor = MeshComp->GetOwner())
		{
			LOG_SCREEN("Check Notify");
			FGameplayEventData PlayData;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerGameplayTag, PlayData);
		}
	}
}

FString UAnimNotify_EventTrigger::GetNotifyName_Implementation() const
{
	return TriggerGameplayTag.ToString();
}
