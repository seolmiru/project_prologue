// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_WaitBoolCondition.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBoolConditionMethod);

/**
 * 
 */
UCLASS()
class PROLOGUE_API UAT_WaitBoolCondition : public UAbilityTask
{
	GENERATED_BODY()

public:
	FBoolConditionMethod OnCondition;

	TFunction<bool()> ConditionFunction;

	static UAT_WaitBoolCondition* WaitUntilTrue(UGameplayAbility* OwningAbility, TFunction<bool()> Condition);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

protected:
	virtual void OnDestroy(bool bInOwnerFinished) override;
};
