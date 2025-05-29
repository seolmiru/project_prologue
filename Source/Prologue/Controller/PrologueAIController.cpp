// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueAIController.h"

#include "PrologueAISubsystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Prologue/Prologue.h"

APrologueAIController::APrologueAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
	AISenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("EnemySenseConfig_Sight");
	AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = false;
	AISenseConfig_Sight->SightRadius = 1000.f;
	AISenseConfig_Sight->LoseSightRadius = 0.f;
	AISenseConfig_Sight->PeripheralVisionAngleDegrees = 360.f;

	EnemyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("EnemyPerceptionComponent");
	EnemyPerceptionComponent->ConfigureSense(*AISenseConfig_Sight);
	EnemyPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());
	EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnEnemyPerceptionUpdated);

	SetGenericTeamId(FGenericTeamId(1));
}

ETeamAttitude::Type APrologueAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* PawnToCheck = Cast<const APawn>(&Other);

	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(PawnToCheck->GetController());

	if (OtherTeamAgent && OtherTeamAgent->GetGenericTeamId() < GetGenericTeamId())
	{
		return ETeamAttitude::Hostile;
	}

	return ETeamAttitude::Friendly;
}

void APrologueAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UPrologueAISubsystem* AISubsystem = GetWorld()->GetSubsystem<UPrologueAISubsystem>())
	{
		AISubsystem->UnRegisterAIController(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void APrologueAIController::ReceiveCombatAlert(AActor* TargetPlayer)
{
	if (!TargetPlayer)
	{
		return;
	}

	// 전투 알림을 받았을 때, 즉시 플레이어를 TargetActor로 지정
	Blackboard->SetValueAsObject(FName("TargetActor"), TargetPlayer);
}

void APrologueAIController::InitiateCombat(AActor* TargetPlayer)
{
	// 플레이어 발견 시 주변 AI들에게 전투 알림 전송
	if (UPrologueAISubsystem* AISubsystem = GetWorld()->GetSubsystem<UPrologueAISubsystem>())
	{
		AISubsystem->TriggerCombatAlert(this, TargetPlayer, CombatAlertRadius);
	}
}

void APrologueAIController::BeginPlay()
{
	Super::BeginPlay();

	if (UPrologueAISubsystem* AISubsystem = GetWorld()->GetSubsystem<UPrologueAISubsystem>())
	{
		AISubsystem->RegisterAIController(this);
	}
	
	if (UCrowdFollowingComponent* CrowdComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		CrowdComp->SetCrowdSimulationState(bEnableDetourCrowdAvoidance ? ECrowdSimulationState::Enabled : ECrowdSimulationState::Disabled);

		switch (DetourCrowdAvoidanceQuality)
		{
		case 1 : CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Low);
			break;
		case 2 : CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Medium);
			break;
		case 3 : CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);
			break;
		case 4 : CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);
			break;
		default:
			break;
		}

		CrowdComp->SetAvoidanceGroup(1);
		CrowdComp->SetGroupsToAvoid(1);
		CrowdComp->SetCrowdCollisionQueryRange(CollisionQueryRange);
	}
}

void APrologueAIController::OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		if (Stimulus.WasSuccessfullySensed() && Actor)
		{
			if (!BlackboardComponent->GetValueAsObject(FName("TargetActor")))
			{
				InitiateCombat(Actor);
			}

			BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
		}
		else
		{
			BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
		}
	}
}
