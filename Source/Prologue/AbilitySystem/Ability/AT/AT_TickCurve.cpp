// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_TickCurve.h"

UAT_TickCurve::UAT_TickCurve()
{
	bTickingTask = true;
	ElapsedTime = 0.f;
}

UAT_TickCurve* UAT_TickCurve::CreateTask(UGameplayAbility* OwningAbility, UCurveFloat* CurveFloat)
{
	UAT_TickCurve* NewTask = NewAbilityTask<UAT_TickCurve>(OwningAbility);
	NewTask->CurveFloat = CurveFloat;
	return NewTask;
}

void UAT_TickCurve::Activate()
{
	Super::Activate();
}

void UAT_TickCurve::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	ElapsedTime += DeltaTime;

	if (OnCurveTick.IsBound())
	{
		OnCurveTick.Broadcast(CurveFloat->GetFloatValue(ElapsedTime));
		if (CurveFloat->GetFloatValue(ElapsedTime) >= 1.f)
		{
			if (OnComplete.IsBound())
			{
				OnComplete.Broadcast();
			}
		}
	}
}
