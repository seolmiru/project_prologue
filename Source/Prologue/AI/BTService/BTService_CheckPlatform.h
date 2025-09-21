// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckPlatform.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UBTService_CheckPlatform : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CheckPlatform();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PlayerActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsOnSamePlatformKey;
};