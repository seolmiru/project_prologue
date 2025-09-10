// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ActivateWeightedAbilityAndWait.generated.h"

USTRUCT()
struct FWeightedAbilityTag
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FGameplayTag AbilityTag;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float Weight = 1.f;
};

/**
 * 
 */
UCLASS()
class PROLOGUE_API UBTTask_ActivateWeightedAbilityAndWait : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ActivateWeightedAbilityAndWait();

protected:
	UPROPERTY(EditAnywhere)
	TArray<FWeightedAbilityTag> WeightedAbilityTags;

	FGameplayTag ActivatedAbilityTag;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);

	void CleanUp();

	TWeakObjectPtr<UBehaviorTreeComponent> OwnerCompWeakPtr;

	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	FDelegateHandle OnAbilityEndedDelegate;
};
