// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA_MontageAbility.h"
#include "GA_CommaDash.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_CommaDash : public UGA_MontageAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable)
	void OnCurveTick(float Alpha);
	
protected:
	virtual void OnComplete() override;

	UFUNCTION(BlueprintCallable, Category = "Dash|GroundCheck")
	bool IsSafeLandingZone(const FVector& CandidateLocation, const TArray<AActor*>& IgnoreActors, FVector& OutAdjustedLocation) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveLength = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|GroundCheck")
	float GroundTraceUpOffset = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|GroundCheck")
	float GroundTraceDistance = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|GroundCheck")
	float TargetZOffset = 2.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|GroundCheck")
	int32 PathCheckSteps = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float MinDashDistance = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Vertical")
	bool bAllowVerticalDash = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Vertical", meta = (EditCondition = "bAllowVerticalDash", ClampMin = "0.0", UIMin = "0.0"))
	float MaxAscendHeight = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Vertical", meta = (EditCondition = "bAllowVerticalDash", ClampMin = "0.0", UIMin = "0.0"))
	float VerticalCheckTraceLength = 400.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	TObjectPtr<class UCurveFloat> Curve;

	FVector TargetPos;
	FVector BasePos;
};
