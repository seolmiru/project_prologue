// Copyright Epic Games, Inc. All Rights Reserved.

#include "GA_CommaDash.h"
#include "AbilitySystemComponent.h"
#include "NavigationSystem.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AT/AT_TickCurve.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Prologue/Character/Player/Comma.h"
#include "Prologue/Controller/CommaController.h"
#include "DrawDebugHelpers.h"

UGA_CommaDash::UGA_CommaDash()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_CommaDash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    CachedCommaCharacter = CastChecked<AComma>(GetAvatarActorFromActorInfo());
    CachedNavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

    if (!CachedCommaCharacter || !CachedNavSystem)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    ApplyDashEffects();
    
    UAbilityTask_PlayMontageAndWait* PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("DashMontage"), AnimMontage, 1.0f);
    PlayTask->OnCompleted.AddDynamic(this, &UGA_CommaDash::OnComplete);
    PlayTask->OnInterrupted.AddDynamic(this, &UGA_CommaDash::OnInterrupted);
    PlayTask->ReadyForActivation();

    StartPosition = CachedCommaCharacter->GetActorLocation();

    FVector DesiredDirection = CalculateDesiredDirection();

    FDashTargetInfo TargetInfo;
    bool bFoundTarget = FindBestDashTarget(DesiredDirection, TargetInfo);

    if (!bFoundTarget)
    {
        bFoundTarget = ScanForAlternativeTargets(DesiredDirection, TargetInfo);
    }

    if (bFoundTarget && TargetInfo.bIsValid)
    {
        TargetPosition = TargetInfo.TargetLocation;
        ActualTargetPosition = TargetPosition;

        // 캡슐 높이 보정
        const float CapsuleHalfHeight = CachedCommaCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
        ActualTargetPosition.Z += CapsuleHalfHeight + TargetZOffset;

        // 최소 거리 검증
        if (FVector::Dist(StartPosition, ActualTargetPosition) < MinDashDistance)
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }

        if (MovementCurve)
        {
            UAT_TickCurve* TickCurveTask = UAT_TickCurve::CreateTask(this, MovementCurve);
            if (TickCurveTask)
            {
                TickCurveTask->OnCurveTick.AddDynamic(this, &UGA_CommaDash::OnCurveTick);
                TickCurveTask->OnComplete.AddDynamic(this, &UGA_CommaDash::OnComplete);
                TickCurveTask->ReadyForActivation();
            }
        }

        if (bEnableDebugVisualization)
        {
            DrawDebugVisualization(StartPosition, ActualTargetPosition, true);
        }
    }
    else
    {
        if (bEnableDebugVisualization)
        {
            DrawDebugVisualization(StartPosition, StartPosition + DesiredDirection * MoveLength, false);
        }
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    }
}

void UGA_CommaDash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
    
    CachedCommaCharacter = nullptr;
    CachedNavSystem = nullptr;
}

FVector UGA_CommaDash::CalculateDesiredDirection() const
{
    if (!CachedCommaCharacter)
        return FVector::ForwardVector;

    ACommaController* Controller = Cast<ACommaController>(CachedCommaCharacter->GetController());
    if (!Controller)
        return CachedCommaCharacter->GetActorForwardVector();

    FVector2D MovementInput = CachedCommaCharacter->GetCachedMovementInput();
    
    if (MovementInput.IsNearlyZero())
    {
        // 입력이 없으면 캐릭터 전방 방향
        return CachedCommaCharacter->GetActorForwardVector();
    }

    // 카메라 기준 월드 방향 계산
    const FRotator ControlRotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, ControlRotation.Yaw, 0);
    const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    return (ForwardDir * MovementInput.Y + RightDir * MovementInput.X).GetSafeNormal();
}

bool UGA_CommaDash::FindBestDashTarget(const FVector& DesiredDirection, FDashTargetInfo& OutTargetInfo)
{
    TArray<FVector> SafePositions;
    FVector TargetPos = StartPosition + DesiredDirection * MoveLength;
    
    // 전체 경로 검증
    if (!ValidateDashPath(StartPosition, TargetPos, SafePositions))
    {
        return false;
    }

    // 안전한 위치 중 가장 먼 곳 선택
    float MaxDistance = 0.f;
    FVector BestPosition = StartPosition;
    bool bFoundValidTarget = false;

    for (const FVector& SafePos : SafePositions)
    {
        float Distance = FVector::Dist(StartPosition, SafePos);
        
        if (bAllowPartialDash)
        {
            float PercentageAchieved = Distance / MoveLength;
            if (PercentageAchieved < PartialDashMinPercentage)
                continue;
        }

        if (Distance > MaxDistance && Distance >= MinDashDistance)
        {
            MaxDistance = Distance;
            BestPosition = SafePos;
            bFoundValidTarget = true;
        }
    }

    if (bFoundValidTarget)
    {
        OutTargetInfo.TargetLocation = BestPosition;
        OutTargetInfo.Distance = MaxDistance;
        OutTargetInfo.SafetyScore = CalculatePlatformSafetyScore(BestPosition);
        OutTargetInfo.bIsValid = true;
        return true;
    }

    return false;
}

bool UGA_CommaDash::ValidateDashPath(const FVector& StartPos, const FVector& EndPos, TArray<FVector>& OutSafePositions)
{
    OutSafePositions.Empty();
    
    const int32 NumSteps = FMath::Max(1, PathCheckSteps);
    const float StepSize = 1.0f / NumSteps;
    
    for (int32 Step = 1; Step <= NumSteps; ++Step)
    {
        float Alpha = Step * StepSize;
        FVector TestLocation = FMath::Lerp(StartPos, EndPos, Alpha);
        
        FVector AdjustedLocation;
        if (IsValidPlatform(TestLocation, AdjustedLocation))
        {
            OutSafePositions.Add(AdjustedLocation);
        }
    }
    
    return OutSafePositions.Num() > 0;
}

bool UGA_CommaDash::ScanForAlternativeTargets(const FVector& DesiredDirection, FDashTargetInfo& OutTargetInfo)
{
    if (NumFOVTracesPerSide <= 0 || FOVAngleDegrees <= 0.f)
        return false;

    TArray<FDashTargetInfo> PotentialTargets;
    const FVector ForwardDir = CachedCommaCharacter->GetActorForwardVector();
    const float AngleIncrement = (FOVAngleDegrees * 0.5f) / NumFOVTracesPerSide;

    for (int32 Side = -1; Side <= 1; Side += 2)
    {
        for (int32 TraceIndex = 1; TraceIndex <= NumFOVTracesPerSide; ++TraceIndex)
        {
            float CurrentAngle = AngleIncrement * TraceIndex * Side;
            FRotator Rotation(0, CurrentAngle, 0);
            FVector ScanDirection = Rotation.RotateVector(DesiredDirection);
            
            FDashTargetInfo TargetInfo;
            if (FindBestDashTarget(ScanDirection, TargetInfo))
            {
                PotentialTargets.Add(TargetInfo);
            }
        }
    }

    // 가장 가까운 안전한 타겟 선택
    if (PotentialTargets.Num() > 0)
    {
        PotentialTargets.Sort([this](const FDashTargetInfo& A, const FDashTargetInfo& B)
        {
            if (FMath::Abs(A.SafetyScore - B.SafetyScore) > 0.1f)
                return A.SafetyScore > B.SafetyScore;
            
            return A.Distance < B.Distance;
        });

        OutTargetInfo = PotentialTargets[0];
        return true;
    }

    return false;
}

bool UGA_CommaDash::IsValidPlatform(const FVector& TestLocation, FVector& OutAdjustedLocation) const
{
    FHitResult HitResult;
    if (!PerformPlatformTrace(TestLocation, HitResult))
        return false;

    // 착지 지점 조정
    OutAdjustedLocation = HitResult.ImpactPoint;
    
    FNavLocation NavLocation;
    const FVector QueryExtent(PlatformDetectionRadius, PlatformDetectionRadius, MaxPlatformHeightDiff);
    
    if (!CachedNavSystem->ProjectPointToNavigation(OutAdjustedLocation, NavLocation, QueryExtent))
        return false;
    
    OutAdjustedLocation = NavLocation.Location;
    
    FVector SafeCenter;
    if (GetPlatformSafeCenter(OutAdjustedLocation, SafeCenter))
    {
        // 현재 위치에서 안전한 중심점 방향으로 SafetyMargin만큼 이동
        FVector ToCenterDir = (SafeCenter - OutAdjustedLocation).GetSafeNormal2D();
        if (!ToCenterDir.IsNearlyZero())
        {
            OutAdjustedLocation += ToCenterDir * SafetyMargin;
            
            // 조정된 위치가 여전히 플랫폼 위에 있는지 재검증
            FHitResult ValidationHit;
            FVector ValidationStart = OutAdjustedLocation + FVector(0, 0, 50.0f);
            FVector ValidationEnd = OutAdjustedLocation - FVector(0, 0, 10.0f);
            
            TArray<AActor*> ActorsToIgnore;
            ActorsToIgnore.Add(CachedCommaCharacter);
            
            if (!UKismetSystemLibrary::LineTraceSingle(
                GetWorld(),
                ValidationStart,
                ValidationEnd,
                UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
                false,
                ActorsToIgnore,
                EDrawDebugTrace::None,
                ValidationHit,
                true))
            {
                OutAdjustedLocation = HitResult.ImpactPoint;
            }
        }
    }
    
    // 최종 착지 지점이 캐릭터가 안전하게 설 수 있는지 확인
    const float CapsuleRadius = CachedCommaCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
    
    FHitResult CenterHit;
    FVector CenterTraceStart = OutAdjustedLocation + FVector(0, 0, 50.0f);
    FVector CenterTraceEnd = OutAdjustedLocation - FVector(0, 0, 10.0f);
    
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(CachedCommaCharacter);
    
    bool bCenterValid = UKismetSystemLibrary::SphereTraceSingle(
        GetWorld(),
        CenterTraceStart,
        CenterTraceEnd,
        CapsuleRadius,
        UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
        false,
        ActorsToIgnore,
        EDrawDebugTrace::None,
        CenterHit,
        true
    );
    
    return bCenterValid;
}

bool UGA_CommaDash::PerformPlatformTrace(const FVector& TestLocation, FHitResult& OutHit) const
{
    const float TraceStartZ = TestLocation.Z + MaxPlatformHeightDiff;
    const float TraceEndZ = TestLocation.Z - MaxPlatformHeightDiff;
    
    FVector TraceStart(TestLocation.X, TestLocation.Y, TraceStartZ);
    FVector TraceEnd(TestLocation.X, TestLocation.Y, TraceEndZ);
    
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(CachedCommaCharacter);
    
    return UKismetSystemLibrary::SphereTraceSingle(
        GetWorld(),
        TraceStart,
        TraceEnd,
        PlatformDetectionRadius,
        UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
        false,
        ActorsToIgnore,
        bEnableDebugVisualization ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
        OutHit,
        true,
        FLinearColor::Red,
        FLinearColor::Green,
        DebugDrawDuration
    );
}

float UGA_CommaDash::CalculatePlatformSafetyScore(const FVector& PlatformLocation) const
{
    float SafetyScore = 1.0f;
    
    float HeightDiff = FMath::Abs(PlatformLocation.Z - StartPosition.Z);
    float HeightPreference = 1.0f - FMath::Clamp(HeightDiff / (MaxPlatformHeightDiff * 2.0f), 0.0f, 0.3f);
    SafetyScore *= HeightPreference;
    
    FNavLocation NavLocation;
    if (CachedNavSystem->ProjectPointToNavigation(PlatformLocation, NavLocation))
    {
        float NavDistance = FVector::Dist(PlatformLocation, NavLocation.Location);
        if (NavDistance < 5.0f)
        {
            SafetyScore *= 1.1f;
        }
    }
    
    return FMath::Clamp(SafetyScore, 0.1f, 1.0f);
}

void UGA_CommaDash::ApplyDashEffects()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC) return;

    if (JustDashTimingEffect)
    {
        FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
        ContextHandle.AddSourceObject(this);
        FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(JustDashTimingEffect, 0.f, ContextHandle);
        ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }

    if (InvincibleEffect)
    {
        FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
        ContextHandle.AddSourceObject(this);
        FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(InvincibleEffect, 0.f, ContextHandle);
        ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }
}

void UGA_CommaDash::DrawDebugVisualization(const FVector& StartPos, const FVector& EndPos, bool bSuccess) const
{
    if (!bEnableDebugVisualization) return;

    FColor PathColor = bSuccess ? FColor::Green : FColor::Red;
    
    DrawDebugLine(GetWorld(), StartPos, EndPos, PathColor, false, DebugDrawDuration, 0, 5.0f);
    
    DrawDebugSphere(GetWorld(), StartPos, 20.0f, 12, FColor::Blue, false, DebugDrawDuration);
    DrawDebugSphere(GetWorld(), EndPos, 20.0f, 12, PathColor, false, DebugDrawDuration);
    
    if (FOVAngleDegrees > 0 && NumFOVTracesPerSide > 0)
    {
        FVector ForwardDir = CachedCommaCharacter->GetActorForwardVector();
        float HalfFOVRad = FMath::DegreesToRadians(FOVAngleDegrees * 0.5f);
        
        FVector LeftBound = ForwardDir.RotateAngleAxis(-FOVAngleDegrees * 0.5f, FVector::UpVector) * MoveLength;
        FVector RightBound = ForwardDir.RotateAngleAxis(FOVAngleDegrees * 0.5f, FVector::UpVector) * MoveLength;
        
        DrawDebugLine(GetWorld(), StartPos, StartPos + LeftBound, FColor::Yellow, false, DebugDrawDuration, 0, 2.0f);
        DrawDebugLine(GetWorld(), StartPos, StartPos + RightBound, FColor::Yellow, false, DebugDrawDuration, 0, 2.0f);
    }
}

void UGA_CommaDash::OnCurveTick(float Alpha)
{
    if (!CachedCommaCharacter) return;
    
    FVector InterpolatedLocation = FMath::Lerp(StartPosition, ActualTargetPosition, Alpha);
    CachedCommaCharacter->SetActorLocation(InterpolatedLocation);
}

bool UGA_CommaDash::GetPlatformSafeCenter(const FVector& PlatformLocation, FVector& OutSafeCenter) const
{
    const float ScanRadius = 100.0f;
    const int32 NumScans = 8;
    
    TArray<FVector> ValidPoints;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(CachedCommaCharacter);
    
    // 8방향으로 스캔하여 플랫폼의 대략적인 중심 찾기
    for (int32 i = 0; i < NumScans; ++i)
    {
        float Angle = (2.0f * PI * i) / NumScans;
        FVector ScanDirection(FMath::Cos(Angle), FMath::Sin(Angle), 0);
        FVector ScanLocation = PlatformLocation + ScanDirection * ScanRadius;
        
        FHitResult ScanHit;
        if (PerformPlatformTrace(ScanLocation, ScanHit))
        {
            ValidPoints.Add(ScanHit.ImpactPoint);
        }
    }
    
    if (ValidPoints.Num() < 3)
    {
        OutSafeCenter = PlatformLocation;
        return false;
    }
    
    // 유효한 점들의 평균 위치를 안전한 중심으로 계산
    FVector CenterSum = FVector::ZeroVector;
    for (const FVector& Point : ValidPoints)
    {
        CenterSum += Point;
    }
    
    OutSafeCenter = CenterSum / ValidPoints.Num();
    OutSafeCenter.Z = PlatformLocation.Z;
    
    return true;
}

void UGA_CommaDash::OnComplete()
{
    if (CachedCommaCharacter)
    {
        // 최종 위치 안전성 검증
        UCharacterMovementComponent* MovementComp = CachedCommaCharacter->GetCharacterMovement();
        if (MovementComp)
        {
            MovementComp->Velocity = FVector::ZeroVector;
            MovementComp->UpdateFloorFromAdjustment();
            
            // 바닥이 없거나 불안정한 경우 추가 조정
            if (!MovementComp->IsMovingOnGround() || MovementComp->IsFalling())
            {
                FVector CurrentLocation = CachedCommaCharacter->GetActorLocation();
                FVector SafeLocation;
                
                // 현재 위치에서 가장 가까운 안전한 플랫폼 찾기
                bool bFoundSafety = false;
                const float SearchRadius = 150.0f;
                const int32 SearchSteps = 8;
                
                for (int32 i = 0; i < SearchSteps && !bFoundSafety; ++i)
                {
                    float Angle = (2.0f * PI * i) / SearchSteps;
                    FVector SearchDir(FMath::Cos(Angle), FMath::Sin(Angle), 0);
                    FVector SearchLocation = CurrentLocation + SearchDir * (SafetyMargin + 20.0f);
                    
                    if (IsValidPlatform(SearchLocation, SafeLocation))
                    {
                        // 캡슐 높이 보정
                        const float CapsuleHalfHeight = CachedCommaCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
                        SafeLocation.Z += CapsuleHalfHeight;
                        
                        CachedCommaCharacter->SetActorLocation(SafeLocation);
                        MovementComp->Velocity = FVector::ZeroVector;
                        bFoundSafety = true;
                    }
                }
                
                /*if (!bFoundSafety)
                {
                    CachedCommaCharacter->SetActorLocation(StartPosition);
                    MovementComp->Velocity = FVector::ZeroVector;
                }*/
            }
            
            // 착지 후 추가적인 위치 미세 조정
            FVector FinalLocation = CachedCommaCharacter->GetActorLocation();
            FVector AdjustedFinalLocation;
            if (IsValidPlatform(FinalLocation, AdjustedFinalLocation))
            {
                // 플랫폼 중심 방향으로 약간 더 이동
                FVector SafeCenter;
                if (GetPlatformSafeCenter(AdjustedFinalLocation, SafeCenter))
                {
                    FVector ToCenterDir = (SafeCenter - FinalLocation).GetSafeNormal2D();
                    if (!ToCenterDir.IsNearlyZero())
                    {
                        FVector ExtraAdjustment = FinalLocation + ToCenterDir * (SafetyMargin * 0.5f);
                        
                        FHitResult ValidationHit;
                        if (PerformPlatformTrace(ExtraAdjustment, ValidationHit))
                        {
                            const float CapsuleHalfHeight = CachedCommaCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
                            ExtraAdjustment.Z = ValidationHit.ImpactPoint.Z + CapsuleHalfHeight;
                            CachedCommaCharacter->SetActorLocation(ExtraAdjustment);
                        }
                    }
                }
            }
        }
    }
    
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_CommaDash::OnInterrupted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}