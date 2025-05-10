// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TagToString.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UBTTask_TagToString : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_TagToString();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	FGameplayTag Tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	struct FBlackboardKeySelector SelectedAbilityKey;	
};
