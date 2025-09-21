// Fill out your copyright notice in the Description page of Project Settings.


#include "Prologue/AI/BTService/BTService_CheckPlatform.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Prologue/PrologueBlackboardKey.h"
#include "Prologue/PrologueFunctionLibrary.h"

UBTService_CheckPlatform::UBTService_CheckPlatform()
{
	NodeName = "Check Platform";

	Interval = 0.3f;
}

void UBTService_CheckPlatform::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!BlackboardComp || !AIController)
	{
		return;
	}

	APawn* AIPawn = AIController->GetPawn();
	AActor* PlayerActor = Cast<AActor>(BlackboardComp->GetValueAsObject(PlayerActorKey.SelectedKeyName));

	if (!AIPawn || !PlayerActor)
	{
		BlackboardComp->SetValueAsBool(IsOnSamePlatformKey.SelectedKeyName, false);
		return;
	}

	AActor* AIPlatform = UPrologueFunctionLibrary::GetCurrentStandingPlatform(AIPawn);
	AActor* PlayerPlatform = UPrologueFunctionLibrary::GetCurrentStandingPlatform(PlayerActor);

	const bool bIsOnSamePlatform = (AIPlatform != nullptr) && (AIPlatform == PlayerPlatform);

	BlackboardComp->SetValueAsBool(IsOnSamePlatformKey.SelectedKeyName, bIsOnSamePlatform);
}
