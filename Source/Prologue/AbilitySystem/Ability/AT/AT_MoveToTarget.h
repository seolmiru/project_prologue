// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_MoveToTarget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMoveToTargetDelegate);

/**
 * 
 */
UCLASS()
class PROLOGUE_API UAT_MoveToTarget : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FMoveToTargetDelegate OnFinished;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAT_MoveToTarget* MoveToTarget(UGameplayAbility* OwningAbility, FVector TargetLocation, float Duration);

	virtual void Activate() override;

private:
	virtual void TickTask(float DeltaTime) override;

	FVector StartLocation;
	FVector TargetLocation;
	float Duration;
	float ElapsedTime;
};
