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
	float MaxPlatformHeightDiff = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|GroundCheck")
	float TargetZOffset = 2.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|GroundCheck")
	int32 PathCheckSteps = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float MinDashDistance = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float GroundTraceRadius = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|FOV")
	float FOVAngleDegrees = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|FOV")
	int32 NumFOVTracesPerSide = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|FOV")
	bool bDebugFOVTraces = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Safety")
	bool bAllowPartialDash = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Safety")
	float PartialDashMinPercentage = 0.7f;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	TObjectPtr<class UCurveFloat> Curve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TSubclassOf<UGameplayEffect> JustDashTimingEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TSubclassOf<UGameplayEffect> InvincibleEffect;
	
	FVector TargetPos;
	FVector BasePos;
};
