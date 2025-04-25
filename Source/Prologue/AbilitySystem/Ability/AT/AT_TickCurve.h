// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_TickCurve.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTickCurveDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTickCurveDelegate_OneParam, float, Alpha);
/**
 * 
 */
UCLASS()
class PROLOGUE_API UAT_TickCurve : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAT_TickCurve();

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "WaitForTrace", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAT_TickCurve* CreateTask(UGameplayAbility* OwningAbility, UCurveFloat* CurveFloat);

	virtual void Activate() override;

	virtual void TickTask(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	TObjectPtr<class UCurveFloat> CurveFloat;

	FTickCurveDelegate OnComplete;
	FTickCurveDelegate_OneParam OnCurveTick;

	float ElapsedTime;
};
