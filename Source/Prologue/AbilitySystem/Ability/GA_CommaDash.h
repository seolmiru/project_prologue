#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_CommaDash.generated.h"

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
	
	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> AnimMontage;

	UFUNCTION(BlueprintCallable, Category = "Dash|GroundCheck")
	bool IsSafeLandingZone(const FVector& CandidateFeetLocation, const TArray<AActor*>& IgnoreActors, FVector& OutAdjustedFeetLocation) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float MoveLength = 570.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|GroundCheck")
	float MaxPlatformHeightDiff = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|GroundCheck")
	float TargetZOffset = 2.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|GroundCheck")
	int32 PathCheckSteps = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float MinDashDistance = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|GroundCheck")
	float GroundTraceRadius = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|FOV")
	float FOVAngleDegrees = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|FOV")
	int32 NumFOVTracesPerSide = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Debug")
	bool bDebugTrace = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Safety")
	bool bAllowPartialDash = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Safety", meta = (EditCondition = "bAllowPartialDash"))
	float PartialDashMinPercentage = 0.7f;	

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|GroundCheck", AdvancedDisplay)
    float MaxStepHeightForValidGround = 20.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	TObjectPtr<class UCurveFloat> Curve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TSubclassOf<UGameplayEffect> JustDashTimingEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TSubclassOf<UGameplayEffect> InvincibleEffect;
	
	FVector TargetPos;
	FVector BasePos;
};