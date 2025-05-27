// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PrologueAISubsystem.generated.h"

class APrologueAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGlobalCombatStateChanged, bool, bInCombat);

/**
 * 
 */
UCLASS()
class PROLOGUE_API UPrologueAISubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	void RegisterAIController(APrologueAIController* AIController);

	UFUNCTION(BlueprintCallable, Category = "AI")
	void UnRegisterAIController(APrologueAIController* AIController);

	UFUNCTION(BlueprintCallable, Category = "AI")
	void TriggerCombatAlert(APrologueAIController* InitiatorAI, AActor* TargetPlayer, float AlertRadius = 1500.f);

protected:
	UPROPERTY()
	TArray<TWeakObjectPtr<APrologueAIController>> RegisteredAIControllers;

private:
	void CleanUpInvalidControllers();
};
