// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_OrientToTargetActor.generated.h"

// BTService로 Target을 향해 Rotate 시키는 클래스

/**
 * 
 */
UCLASS()
class PROLOGUE_API UBTService_OrientToTargetActor : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_OrientToTargetActor();

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Target")
	FBlackboardKeySelector InTargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Target")
	float RotationInterpSpeed;
};
