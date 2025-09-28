// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ActivateWeightedAbilityAndWait.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"

UBTTask_ActivateWeightedAbilityAndWait::UBTTask_ActivateWeightedAbilityAndWait()
{
	NodeName = "Activate Weighted Ability And Wait";

	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_ActivateWeightedAbilityAndWait::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	if (WeightedAbilityTags.Num() == 0)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AIController->GetPawn())
	{
		return EBTNodeResult::Failed;
	}

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(AIController->GetPawn());
	UAbilitySystemComponent* ASC = ASI ? ASI->GetAbilitySystemComponent() : nullptr;
	if (!ASC)
	{
		return EBTNodeResult::Failed;
	}

	float TotalWeight = 0.f;
	for (const FWeightedAbilityTag& WeightedAbilityTag : WeightedAbilityTags)
	{
		if (WeightedAbilityTag.AbilityTag != LastUsedAbility)
		{
			TotalWeight += WeightedAbilityTag.Weight;
		}
	}

	if (TotalWeight <= 0.f)
	{
		return EBTNodeResult::Failed;
	}

	const float RandomValue = FMath::FRandRange(0.f, TotalWeight);
	float CurrentWeightSum = 0.f;
	FGameplayTag SelectedTag;

	for (const FWeightedAbilityTag& WeightedAbilityTag : WeightedAbilityTags)
	{
		if (!WeightedAbilityTag.AbilityTag.IsValid() || WeightedAbilityTag.AbilityTag == LastUsedAbility)
		{
			continue;
		}
		
		CurrentWeightSum += WeightedAbilityTag.Weight;
		if (RandomValue <= CurrentWeightSum)
		{
			SelectedTag = WeightedAbilityTag.AbilityTag;
			break;
		}
	}

	if (!SelectedTag.IsValid())
	{
		return EBTNodeResult::Failed;
	}

	CleanUp();

	OwnerCompWeakPtr = &OwnerComp;
	AbilitySystemComponent = ASC;
	ActivatedAbilityTag = SelectedTag;

	OnAbilityEndedDelegate = ASC->OnAbilityEnded.AddUObject(this, &UBTTask_ActivateWeightedAbilityAndWait::OnAbilityEnded);

	if (ASC->TryActivateAbilitiesByTag(SelectedTag.GetSingleTagContainer()))
	{
		return EBTNodeResult::InProgress;
	}

	CleanUp();
	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTTask_ActivateWeightedAbilityAndWait::AbortTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	CleanUp();
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_ActivateWeightedAbilityAndWait::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	CleanUp();
}

void UBTTask_ActivateWeightedAbilityAndWait::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	if (UBehaviorTreeComponent* OwnerComp = OwnerCompWeakPtr.Get())
	{
		if (AbilityEndedData.AbilityThatEnded && AbilityEndedData.AbilityThatEnded->GetAssetTags().HasTag(ActivatedAbilityTag))
		{
			LastUsedAbility = ActivatedAbilityTag;
			
			CleanUp();
			FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}

void UBTTask_ActivateWeightedAbilityAndWait::CleanUp()
{
	if (AbilitySystemComponent.IsValid() && OnAbilityEndedDelegate.IsValid())
	{
		AbilitySystemComponent->OnAbilityEnded.Remove(OnAbilityEndedDelegate);
	}

	ActivatedAbilityTag = FGameplayTag::EmptyTag;
	OnAbilityEndedDelegate.Reset();
}
