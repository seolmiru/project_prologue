// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_CommaDash.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAT_TickCurve;
class AComma;
class UNavigationSystemV1;

USTRUCT()
struct FDashTargetInfo
{
    GENERATED_BODY()

    FVector TargetLocation;
    float Distance;
    float SafetyScore;
    bool bIsValid;

    FDashTargetInfo()
    {
        TargetLocation = FVector::ZeroVector;
        Distance = 0.f;
        SafetyScore = 0.f;
        bIsValid = false;
    }
};

/**
 * CommaDash Ability - 플레이어의 빠른 이동을 담당하는 대시 어빌리티
 * 공중 플랫폼 환경에서 안전한 착지를 보장하는 스마트 대시 시스템
 */
UCLASS()
class PROLOGUE_API UGA_CommaDash : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UGA_CommaDash();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    // Curve Tick 이벤트 핸들러
    UFUNCTION()
    void OnCurveTick(float Alpha);
    
    // 몽타주 완료 핸들러
    UFUNCTION()
    void OnComplete();

    // 몽타주 중단 핸들러
    UFUNCTION()
    void OnInterrupted();

    // 대시 타겟 계산 함수들
    FVector CalculateDesiredDirection() const;
    bool FindBestDashTarget(const FVector& DesiredDirection, FDashTargetInfo& OutTargetInfo);
    bool ValidateDashPath(const FVector& StartPos, const FVector& EndPos, TArray<FVector>& OutSafePositions);
    bool ScanForAlternativeTargets(const FVector& DesiredDirection, FDashTargetInfo& OutTargetInfo);
    
    // 플랫폼 검증 함수들
    bool IsValidPlatform(const FVector& TestLocation, FVector& OutAdjustedLocation) const;
    bool PerformPlatformTrace(const FVector& TestLocation, FHitResult& OutHit) const;
    float CalculatePlatformSafetyScore(const FVector& PlatformLocation) const;
    
    // 유틸리티 함수들
    void ApplyDashEffects();
    void DrawDebugVisualization(const FVector& StartPos, const FVector& EndPos, bool bSuccess) const;
    bool GetPlatformSafeCenter(const FVector& PlatformLocation, FVector& OutSafeCenter) const;

private:
    // Animation
    UPROPERTY(EditAnywhere, Category = "Dash|Animation")
    TObjectPtr<UAnimMontage> AnimMontage;

    // Movement Parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Movement", meta = (AllowPrivateAccess = "true"))
    float MoveLength = 500.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Movement", meta = (AllowPrivateAccess = "true"))
    float MinDashDistance = 100.f;

    // Platform Detection
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Platform Detection", meta = (AllowPrivateAccess = "true"))
    float MaxPlatformHeightDiff = 800.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Platform Detection", meta = (AllowPrivateAccess = "true"))
    float PlatformDetectionRadius = 50.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Platform Detection", meta = (AllowPrivateAccess = "true"))
    float TargetZOffset = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Platform Detection", meta = (AllowPrivateAccess = "true"))
    int32 PathCheckSteps = 10;

    // FOV Scanning
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|FOV", meta = (AllowPrivateAccess = "true"))
    float FOVAngleDegrees = 90.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|FOV", meta = (AllowPrivateAccess = "true"))
    int32 NumFOVTracesPerSide = 3;

    // Safety Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Safety", meta = (AllowPrivateAccess = "true"))
    bool bAllowPartialDash = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Safety", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0"))
    float PartialDashMinPercentage = 0.7f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Safety", meta = (AllowPrivateAccess = "true"))
    float SafetyMargin = 20.f;

    // Curve
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash|Timeline", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class UCurveFloat> MovementCurve;

    // Effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Effects", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UGameplayEffect> JustDashTimingEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Effects", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UGameplayEffect> InvincibleEffect;

    // Debug
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Debug", meta = (AllowPrivateAccess = "true"))
    bool bEnableDebugVisualization = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Debug", meta = (AllowPrivateAccess = "true"))
    float DebugDrawDuration = 2.0f;

    // Runtime Data
    FVector StartPosition;
    FVector TargetPosition;
    FVector ActualTargetPosition;
    
    UPROPERTY()
    AComma* CachedCommaCharacter;
    
    UPROPERTY()
    UNavigationSystemV1* CachedNavSystem;
};