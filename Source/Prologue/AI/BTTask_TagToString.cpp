// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TagToString.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TagToString::UBTTask_TagToString()
{
	NodeName = TEXT("TagToString");
}

EBTNodeResult::Type UBTTask_TagToString::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsString(SelectedAbilityKey.SelectedKeyName, Tags.ToString());
	}
	else
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Succeeded;
}
