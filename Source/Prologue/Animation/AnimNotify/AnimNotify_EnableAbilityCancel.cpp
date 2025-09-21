// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_EnableAbilityCancel.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Prologue/PrologueGameplayTags.h"

UAnimNotify_EnableAbilityCancel::UAnimNotify_EnableAbilityCancel()
{
}

void UAnimNotify_EnableAbilityCancel::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(MeshComp->GetOwner()))
		{
			if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
			{
				ASC->RemoveLooseGameplayTag(PrologueGameplayTags::Comma_State_CancelDisabled);
				ASC->AddLooseGameplayTag(CancelEnableTag);				
			}
		}
	}
}
