// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_TickBoxTrace.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTraceResultDelegate_OneParam, const FGameplayAbilityTargetDataHandle&, TargetDataHandle);

/**
 * 
 */
UCLASS()
class PROLOGUE_API UAT_TickBoxTrace : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Ability|Tasks")
	FTraceResultDelegate_OneParam OnTraceResultCallback;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks")
	static UAT_TickBoxTrace* TickBoxTrace(UGameplayAbility* OwningAbility);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bShowDebug = true;

	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	void TimerTrace();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float TotalDuration;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	FVector BoxHalfSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float TraceLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float DamageInterval;

	float ElapsedTime;

	FTimerHandle Timer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	TMap<AActor*, float> TimeMap;
};
