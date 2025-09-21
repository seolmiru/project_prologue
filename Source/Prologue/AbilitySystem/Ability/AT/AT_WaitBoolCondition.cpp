// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_WaitBoolCondition.h"

UAT_WaitBoolCondition* UAT_WaitBoolCondition::WaitUntilTrue(UGameplayAbility* OwningAbility,
	TFunction<bool()> Condition)
{
	UAT_WaitBoolCondition* Task = NewAbilityTask<UAT_WaitBoolCondition>(OwningAbility);
	Task->ConditionFunction = Condition;
	return Task;
}

void UAT_WaitBoolCondition::Activate()
{
	Super::Activate();
	bTickingTask = true;
}

void UAT_WaitBoolCondition::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (ConditionFunction && ConditionFunction())
	{
		OnCondition.Broadcast();
		EndTask();
	}
}

void UAT_WaitBoolCondition::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
	bTickingTask = false;
}
