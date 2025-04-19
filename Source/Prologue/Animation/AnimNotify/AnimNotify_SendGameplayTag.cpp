// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SendGameplayTag.h"
#include "AbilitySystemBlueprintLibrary.h"

bool UAnimNotify_SendGameplayTag::Received_Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference) const
{
	if (!MeshComp) return false;

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor) return false;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EventTag, FGameplayEventData());

	return true;
}
