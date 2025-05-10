// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ActivateAbilityByTag.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ActivateAbilityByTag::UBTTask_ActivateAbilityByTag()
{
	NodeName = TEXT("Activate Ability By Tag");
}

EBTNodeResult::Type UBTTask_ActivateAbilityByTag::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	FString TagString = BlackboardComp->GetValueAsString(SelectedTagKey.SelectedKeyName);

	FGameplayTag AbilityTag = FGameplayTag::RequestGameplayTag(FName(*TagString));

	AActor* AIPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;

	UAbilitySystemComponent* ASC = AIPawn->FindComponentByClass<UAbilitySystemComponent>();

	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(AbilityTag);

	bool bActivated = ASC->TryActivateAbilitiesByTag(TagContainer);
	if (!bActivated)
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Succeeded;
}
