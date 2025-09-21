// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ActivateAbilityAndWait.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Prologue/Controller/PrologueAIController.h"

UBTTask_ActivateAbilityAndWait::UBTTask_ActivateAbilityAndWait()
{
	NodeName = "Activate Ability And Wait";

	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_ActivateAbilityAndWait::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AIController->GetPawn())
	{
		return EBTNodeResult::Failed;
	}

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(AIController->GetPawn());
	if (!ASI)
	{
		return EBTNodeResult::Failed;
	}

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC)
	{
		return EBTNodeResult::Failed;
	}

	CleanUp();

	OwnerCompWeakPtr = &OwnerComp;
	AbilitySystemComponent = ASC;

	OnAbilityEndedDelegate = ASC->OnAbilityEnded.AddUObject(this, &UBTTask_ActivateAbilityAndWait::OnAbilityEnded);
	
	if (ASC->TryActivateAbilitiesByTag(AbilityTagToActivate.GetSingleTagContainer()))
	{
		return EBTNodeResult::InProgress;
	}

	CleanUp();
	return EBTNodeResult::Failed;
}

void UBTTask_ActivateAbilityAndWait::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	CleanUp();
}

EBTNodeResult::Type UBTTask_ActivateAbilityAndWait::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CleanUp();
	return EBTNodeResult::Aborted;
}

void UBTTask_ActivateAbilityAndWait::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	if (UBehaviorTreeComponent* OwnerComp = OwnerCompWeakPtr.Get())
	{
		if (AbilityEndedData.AbilityThatEnded && AbilityEndedData.AbilityThatEnded->GetAssetTags().HasTag(AbilityTagToActivate))
		{
			CleanUp();
			FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}

void UBTTask_ActivateAbilityAndWait::CleanUp()
{
	if (AbilitySystemComponent.IsValid() && OnAbilityEndedDelegate.IsValid())
	{
		AbilitySystemComponent->OnAbilityEnded.Remove(OnAbilityEndedDelegate);
	}

	OnAbilityEndedDelegate.Reset();
}
