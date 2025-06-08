// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_TickSocketTrace.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSocketTraceResultDelegate, const FGameplayAbilityTargetDataHandle&, TargetDataHandle);

/**
 * 
 */
UCLASS()
class PROLOGUE_API UAT_TickSocketTrace : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Ability|Tasks")
	FSocketTraceResultDelegate OnTraceResultCallback;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "TickSocketTrace", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAT_TickSocketTrace* TickSocketTrace(UGameplayAbility* OwningAbility);

	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	void TimerTrace();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float TotalDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	FName StartSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	FName EndSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float TraceRadius = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	bool bShowDebug = true;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	float ElapsedTime;
	FTimerHandle Timer;

	TSet<AActor*> HitActors;
};
