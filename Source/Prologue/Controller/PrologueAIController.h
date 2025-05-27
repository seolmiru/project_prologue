// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "PrologueAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle,
	Combat,
	Alerted
};

UCLASS()
class PROLOGUE_API APrologueAIController : public AAIController
{
	GENERATED_BODY()

public:
	APrologueAIController(const FObjectInitializer& ObjectInitializer);

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ReceiveCombatAlert(AActor* TargetPlayer);
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void InitiateCombat(AActor* TargetPlayer);
	
protected:
	virtual void BeginPlay() override;
	
protected:
	UPROPERTY()
	TObjectPtr<UBehaviorTree> BTAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefaultMaxWalkSpeed = 250.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CombatAlertRadius = 1500.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UAIPerceptionComponent* EnemyPerceptionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAISenseConfig_Sight* AISenseConfig_Sight;
	
	UFUNCTION()
	virtual void OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Crowd Config")
	bool bEnableDetourCrowdAvoidance = true;

	UPROPERTY(EditDefaultsOnly, Category = "Crowd Config", meta = (EditCondition = "bEnableDetourCrowdAvoidance", UIMin = "1", UIMax = "4"))
	int32 DetourCrowdAvoidanceQuality = 4;

	UPROPERTY(EditDefaultsOnly, Category = "Crowd Config", meta = (EditCondition = "bEnableDetourCrowdAvoidance"))
	float CollisionQueryRange = 600.f;
};
