// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueAISubsystem.h"

#include "Prologue/Controller/PrologueAIController.h"
#include "Prologue/Prologue.h"

void UPrologueAISubsystem::RegisterAIController(APrologueAIController* AIController)
{
	// AIController 등록, 중복 등록일 경우에는 제외
	if (AIController && !RegisteredAIControllers.Contains(AIController))
	{
		RegisteredAIControllers.Add(AIController);
	}
}

void UPrologueAISubsystem::UnRegisterAIController(APrologueAIController* AIController)
{
	RegisteredAIControllers.RemoveAll([AIController](const TWeakObjectPtr<APrologueAIController>& WeakPtr)
	{
		return WeakPtr.Get() == AIController || !WeakPtr.IsValid();
	});
}

void UPrologueAISubsystem::TriggerCombatAlert(APrologueAIController* InitiatorAI, AActor* TargetPlayer,
	float AlertRadius)
{
	if (!InitiatorAI || !TargetPlayer)
		return;

	CleanUpInvalidControllers();

	const FVector InitiatorLocation = InitiatorAI->GetPawn() ? InitiatorAI->GetPawn()->GetActorLocation() : FVector::ZeroVector;
	int32 AlertedCount = 0;

	// 등록된 모든 AI Controller 중에서 AlertRadius 범위 내에 있는 AI 들에게 전투 시작 알림 전송
	for (const TWeakObjectPtr<APrologueAIController>& WeakAI : RegisteredAIControllers)
	{
		if (APrologueAIController* AI = WeakAI.Get())
		{
			if (AI == InitiatorAI) continue;

			if (APawn* AIPawn = AI->GetPawn())
			{
				const float Dist = FVector::Dist(InitiatorLocation, AIPawn->GetActorLocation());

				if (Dist <= AlertRadius)
				{
					AI->ReceiveCombatAlert(TargetPlayer);
					AlertedCount++;
				}
			}
		}
	}
}

void UPrologueAISubsystem::CleanUpInvalidControllers()
{
	const int32 InitialCount = RegisteredAIControllers.Num();
	
	RegisteredAIControllers.RemoveAll([](const TWeakObjectPtr<APrologueAIController>& WeakPtr)
	{
		return !WeakPtr.IsValid();
	});

	const int32 RemovedCount = InitialCount - RegisteredAIControllers.Num();
	if (RemovedCount > 0)
	{
		LOG_SCREEN_R("Cleaned up %d invalid AI Controllers", RemovedCount);
	}
}
