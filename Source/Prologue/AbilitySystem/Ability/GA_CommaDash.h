// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA_MontageAbility.h"
#include "GA_CommaDash.generated.h"

/**
 * */
UCLASS()
class PROLOGUE_API UGA_CommaDash : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable)
	void OnCurveTick(float Alpha);
	
protected:
	UFUNCTION()
	void OnComplete();

	UFUNCTION()
	void OnInterrupted();

	/** 라인 트레이스에 기반하여 안전한 착지 지점인지 확인하는 함수 */
	UFUNCTION(BlueprintCallable, Category = "Dash|GroundCheck")
	bool IsSafeLandingZone(const FVector& CandidateLocation, const TArray<AActor*>& IgnoreActors, FVector& OutAdjustedLocation) const;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> AnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveLength = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|GroundCheck")
	float MaxPlatformHeightDiff = 800.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|GroundCheck")
	int32 PathCheckSteps = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float MinDashDistance = 100.f;

	/** 지형 검사 시 LineTrace의 간격을 결정합니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|GroundCheck")
	float LineTraceSpread = 100.f;

	/** 지형 검사 그리드의 한 변에 있는 라인 트레이스 수 (홀수로 설정 권장) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|GroundCheck")
	int32 TracesPerSide = 3;

	/** 지형 검사 시 유효한 지점으로 판단하기 위해 필요한 최소 성공 트레이스 비율 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|GroundCheck")
	float MinSuccessTracePercentage = 0.6f;

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