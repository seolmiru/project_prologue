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

	if (!CurveFloat)
	{
		return;
	}

	ElapsedTime += DeltaTime;

	CurveFloat->GetTimeRange(MinTime, MaxTime);
	
	float ClampedTime = FMath::Clamp(ElapsedTime, MinTime, MaxTime);
	
	float CurveValue = FMath::Clamp(CurveFloat->GetFloatValue(ClampedTime), 0.0f, 1.0f);

	if (OnCurveTick.IsBound())
	{
		OnCurveTick.Broadcast(CurveValue);
		
		if (ElapsedTime >= MaxTime || CurveValue >= 1.0f)
		{
			if (OnComplete.IsBound())
			{
				OnComplete.Broadcast();
			}
			
			EndTask();
		}
	}
}