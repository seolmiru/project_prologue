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

struct FPotentialDashTarget
{
    FVector FeetLocation;
    float DistanceSqToStart;

    FPotentialDashTarget(const FVector& InFeetLocation, const FVector& StartActorPos)
        : FeetLocation(InFeetLocation), DistanceSqToStart(FVector::DistSquared(StartActorPos, InFeetLocation)) {}

    bool operator<(const FPotentialDashTarget& Other) const
    {
        return DistanceSqToStart < Other.DistanceSqToStart;
    }
};


void UGA_CommaDash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Just Dash Effect 부여
	FGameplayEffectContextHandle JustDashEffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	JustDashEffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle JustDashEffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(JustDashTimingEffect, 0.f, JustDashEffectContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*JustDashEffectSpecHandle.Data.Get());

	// Invincible Effect 부여
	FGameplayEffectContextHandle InvincibleEffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	InvincibleEffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle InvincibleEffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(InvincibleEffect, 0.f, InvincibleEffectContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*InvincibleEffectSpecHandle.Data.Get());
	
	AComma* Comma = CastChecked<AComma>(GetAvatarActorFromActorInfo());
	UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(GetWorld());
	
	UAbilityTask_PlayMontageAndWait* PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), AnimMontage, 1.0f);
	PlayTask->OnCompleted.AddDynamic(this, &UGA_CommaDash::OnComplete);
	PlayTask->OnInterrupted.AddDynamic(this, &UGA_CommaDash::OnInterrupted);
	PlayTask->ReadyForActivation();
	
	if (!Nav)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UAT_TickCurve* TickCurveTask = UAT_TickCurve::CreateTask(this, Curve);
	if (!TickCurveTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	TickCurveTask->OnCurveTick.AddDynamic(this, &UGA_CommaDash::OnCurveTick);
	TickCurveTask->OnComplete.AddDynamic(this, &UGA_CommaDash::OnComplete);

	FVector ActorStartPos = Comma->GetActorLocation();
	const float CapsuleHalfHeight = Comma->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	// 플레이어의 입력 방향 계산
	ACommaController* Controller = Cast<ACommaController>(Comma->GetController());
	FVector DesiredDirection = FVector::ZeroVector;

	if (Controller)
	{
		FVector2D CachedMovementInput = Comma->GetCachedMovementInput();
		if (!CachedMovementInput.IsNearlyZero())
		{
			// 카메라 회전을 기준으로 월드 좌표계 방향 계산
			const FRotator ControlRotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, ControlRotation.Yaw, 0);
			const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			const FVector RightDir   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			DesiredDirection = (ForwardDir * CachedMovementInput.Y + RightDir * CachedMovementInput.X).GetSafeNormal();
		}
	}

	// 입력이 없을 때는 정면 방향으로 대시
	if (DesiredDirection.IsNearlyZero())
	{
		DesiredDirection = Comma->GetActorForwardVector().GetSafeNormal();
	}

	BasePos = ActorStartPos;
	TargetPos = ActorStartPos;
	bool bSuccessfullyFoundTarget = false;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Comma);

	// 경로 검사 단계 수
	const int32 EffectivePathCheckSteps = FMath::Max(1, PathCheckSteps);
	// SphereTrace 시작 지점 수직 Offset
	const float SphereTraceStartUpOffset = MaxPlatformHeightDiff + 50.f;
	// SphereTrace 종료 지점 수직 Offset
	const float SphereTraceEndDownOffset = MaxPlatformHeightDiff * 2.f + 50.f;

	// 지정된 방향의 지형 추적 후, 안전한 착지 지점을 검사하는 람다 함수
	auto PerformTraceInDirection = [&](const FVector& TraceDirection, FVector& OutValidatedFeetPos, EDrawDebugTrace::Type DebugTraceType, UNavigationSystemV1* NavSystem) -> bool
	{
		// 대시 최종 도착 지점 계산
		FVector CurrentDashAttemptEndPos = ActorStartPos + TraceDirection * MoveLength;
		FVector BestValidFeetPosInDirection = ActorStartPos;
		float FarthestValidDistance = 0.f;
		bool bFoundAnyGroundSpotInThisDirection = false;

		// 안전한 위치들 배열에 저장
		TArray<FVector> ConsecutiveSafePositions;
		bool bLastPositionWasSafe = false;
		
		// MoveLength를 PathCheckSteps로 나누어 단계 별로 검사
		for (int32 StepIndex = 1; StepIndex <= EffectivePathCheckSteps; ++StepIndex)
		{
			const float Alpha = static_cast<float>(StepIndex) / EffectivePathCheckSteps;
			const FVector SamplePosForTrace = FMath::Lerp(ActorStartPos, CurrentDashAttemptEndPos, Alpha);

			// SphereTrace로 바닥 감지
			FHitResult GroundHitResult;
			const bool bHitGround = UKismetSystemLibrary::SphereTraceSingle(
				GetWorld(),
				SamplePosForTrace + FVector(0.f, 0.f, SphereTraceStartUpOffset),
				SamplePosForTrace - FVector(0.f, 0.f, SphereTraceEndDownOffset),
				GroundTraceRadius,
				UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
				false,
				ActorsToIgnore,
				DebugTraceType,
				GroundHitResult,
				true,
				FLinearColor::Red, FLinearColor::Green, 1.0f
			);
			
			if (!bHitGround) 
			{
				bLastPositionWasSafe = false;
				continue;
			}

			FVector GroundImpactPoint = GroundHitResult.ImpactPoint;
			FVector AdjustedSafeFeetPosOnGround;

			if (!IsSafeLandingZone(GroundImpactPoint, ActorsToIgnore, AdjustedSafeFeetPosOnGround)) 
			{
				bLastPositionWasSafe = false;
				continue;
			}

			// NavMesh 검증
			FNavLocation NavMeshLocation;
			const FVector ProjectionExtent(GroundTraceRadius, GroundTraceRadius, MaxPlatformHeightDiff + 100.f);
			
			if (!NavSystem->ProjectPointToNavigation(AdjustedSafeFeetPosOnGround, NavMeshLocation, ProjectionExtent))
			{
				bLastPositionWasSafe = false;
				continue;
			}

			AdjustedSafeFeetPosOnGround = NavMeshLocation.Location;
        
			// 연속된 안전한 위치 추적
			if (bLastPositionWasSafe)
			{
				ConsecutiveSafePositions.Add(AdjustedSafeFeetPosOnGround);
			}
			else
			{
				ConsecutiveSafePositions.Empty();
				ConsecutiveSafePositions.Add(AdjustedSafeFeetPosOnGround);
			}
        
			bLastPositionWasSafe = true;
        
			// 현재 위치까지의 거리 계산
			float CurrentDistance = FVector::Dist(ActorStartPos, AdjustedSafeFeetPosOnGround);
        
			// 더 먼 안전한 위치를 발견했을 때만 업데이트
			if (CurrentDistance > FarthestValidDistance)
			{
				FarthestValidDistance = CurrentDistance;
				BestValidFeetPosInDirection = AdjustedSafeFeetPosOnGround;
				bFoundAnyGroundSpotInThisDirection = true;
			}
		}

		if (bFoundAnyGroundSpotInThisDirection)
		{
			// 부분 대시 허용 시 거리 체크
			if (bAllowPartialDash)
			{
				float AchievedDistance = FVector::Dist(ActorStartPos, BestValidFeetPosInDirection);
				float DesiredDistance = MoveLength;
				float AchievedPercentage = AchievedDistance / DesiredDistance;
        
				if (AchievedPercentage >= PartialDashMinPercentage)
				{
					OutValidatedFeetPos = BestValidFeetPosInDirection;
					return true;
				}
			}
			else
			{
				// 부분 대시를 허용하지 않을 때는 발견한 위치 그대로 사용
				OutValidatedFeetPos = BestValidFeetPosInDirection;
				return true;
			}
		}

		return false;
	};

	// 플레이어가 원하는 방향으로 대시 시도
	FVector ForwardDashTargetFeetLocation;
	EDrawDebugTrace::Type PrimaryTraceDebugType = bDebugFOVTraces ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	if (PerformTraceInDirection(DesiredDirection, ForwardDashTargetFeetLocation, PrimaryTraceDebugType, Nav))
	{
		TargetPos = ForwardDashTargetFeetLocation;
		bSuccessfullyFoundTarget = true;
	}

	// 직선 방향에 땅이 없거나 막혔다면 FOV 범위 내에서 대체 경로 탐색
	if (!bSuccessfullyFoundTarget && NumFOVTracesPerSide > 0 && FOVAngleDegrees > 0.f)
	{
		TArray<FPotentialDashTarget> PotentialTargetsInFOV;
		FVector CharacterActualForwardDir = Comma->GetActorForwardVector().GetSafeNormal();

		float HalfFOV_Rads = FMath::DegreesToRadians(FOVAngleDegrees / 2.0f);
        float AngleIncrementPerTrace = (FOVAngleDegrees / 2.0f) / FMath::Max(1, NumFOVTracesPerSide);


		for (int32 Side = -1; Side <= 1; Side += 2)
		{
			for (int32 TraceIndex = 1; TraceIndex <= NumFOVTracesPerSide; ++TraceIndex)
			{
				float CurrentSampleAngleDegrees = AngleIncrementPerTrace * TraceIndex * Side;
				
				FRotator AngleOffsetRotator(0, CurrentSampleAngleDegrees, 0);
				FVector SampleDirectionForFOV = AngleOffsetRotator.RotateVector(CharacterActualForwardDir);
				
				FVector FOVTargetFeetLocation;
				EDrawDebugTrace::Type FOVTraceDebugType = bDebugFOVTraces ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

				if (PerformTraceInDirection(SampleDirectionForFOV, FOVTargetFeetLocation, FOVTraceDebugType, Nav))
				{
					PotentialTargetsInFOV.Emplace(FOVTargetFeetLocation, ActorStartPos);
				}
			}
		}

		// FOV 내에 안전한 위치가 존재한다면, 그중 가장 가까운 곳을 최종 목표로 설정
		if (PotentialTargetsInFOV.Num() > 0)
		{
			PotentialTargetsInFOV.Sort();
			TargetPos = PotentialTargetsInFOV[0].FeetLocation;
			bSuccessfullyFoundTarget = true;
		}
	}

	if (bSuccessfullyFoundTarget)
	{
		TargetPos.Z += CapsuleHalfHeight + TargetZOffset;
    
		// 최종 검증
		if (FVector::DistSquared(BasePos, TargetPos) < FMath::Square(MinDashDistance))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
    
		TickCurveTask->ReadyForActivation();
	}
	// 모든 검사를 실패했다면 대시 취소
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UGA_CommaDash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_CommaDash::OnCurveTick(float Alpha)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor)
	{
		FVector InterpolatedLocation = FMath::Lerp(BasePos, TargetPos, Alpha);
		AvatarActor->SetActorLocation(InterpolatedLocation);
	}
}

void UGA_CommaDash::OnComplete()
{
	AComma* Comma = CastChecked<AComma>(GetAvatarActorFromActorInfo());
	if (Comma)
	{
		// 캐릭터가 플랫폼 위에 있는지만 확인
		UCharacterMovementComponent* MovementComp = Comma->GetCharacterMovement();
		if (MovementComp)
		{
			// 바닥 업데이트를 통해 낙사 방지 시스템 재활성화
			MovementComp->UpdateFloorFromAdjustment();
            
			// 바닥이 없으면 가장 가까운 안전한 위치로 조정
			if (!MovementComp->IsMovingOnGround())
			{
				FVector CurrentLocation = Comma->GetActorLocation();
				TArray<AActor*> IgnoreActors;
				IgnoreActors.Add(Comma);
                
				FVector AdjustedLocation;
				if (IsSafeLandingZone(CurrentLocation, IgnoreActors, AdjustedLocation))
				{
					Comma->SetActorLocation(AdjustedLocation);
				}
			}
		}
	}
    
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_CommaDash::OnInterrupted()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

// 안전한 착지 위치인지 검증하는 함수
bool UGA_CommaDash::IsSafeLandingZone(const FVector& CandidateLocation, const TArray<AActor*>& IgnoreActors, FVector& OutAdjustedLocation) const
{
	AComma* Comma = CastChecked<AComma>(GetAvatarActorFromActorInfo());
	if (!Comma) return false;

	// 검증을 위한 변수들
	const float CapsuleRadius = Comma->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const float CheckRadius = CapsuleRadius * 1.2f;
	
    const int32 MaxIterations = 2; // 위치 조정 최대 반복 횟수
    const float StepOffset = 5.f; // 위치 조정 시 이동할 거리
    const FVector UpDownOffsetForCheck(0.f, 0.f, MaxPlatformHeightDiff);
    const int32 NumCheckPoints = 6; // 안전한 위치 후보군 주변으로 검사할 지점 수 
    const float MinValidHitRatio = 0.1f; // 최소 유효 충돌 비율

    FVector CurrentCandidate = CandidateLocation;

    for (int32 Iter = 0; Iter < MaxIterations; ++Iter)
    {
        int32 HitCount = 0;
        FVector AccumulatedNonHitOffset = FVector::ZeroVector;

    	// 후보 위치 주변 지점 검사
        for (int32 i = 0; i < NumCheckPoints; ++i)
        {
            float Angle = 2 * PI * (static_cast<float>(i) / NumCheckPoints);
            FVector OffsetFromCandidate = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * CheckRadius;

            FVector TraceStart = CurrentCandidate + OffsetFromCandidate + UpDownOffsetForCheck;
            FVector TraceEnd   = CurrentCandidate + OffsetFromCandidate - UpDownOffsetForCheck;

            FHitResult Hit;
            bool bHit = UKismetSystemLibrary::LineTraceSingle(
                GetWorld(),
                TraceStart,
                TraceEnd,
                UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
                false,
                IgnoreActors,
                bDebugFOVTraces ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
                Hit,
                true
            );

            if (bHit)
            {
                HitCount++;
            }
            else
            {
                AccumulatedNonHitOffset += OffsetFromCandidate;
            }
        }

        float HitRatio = static_cast<float>(HitCount) / static_cast<float>(NumCheckPoints);
        if (HitRatio >= MinValidHitRatio)
        {
            OutAdjustedLocation = CurrentCandidate;
            return true;
        }

        if (HitCount < 2 && Iter > 0)
        {
            break;
        }

    	// Hit에 감지되지 않은 지점들이 있을 때
        if (!AccumulatedNonHitOffset.IsNearlyZero())
        {
        	// 반대 방향으로 후보 위치를 이동 시켜서 안전한 위치로 가게끔 조정
            FVector AdjustmentDirection = -AccumulatedNonHitOffset.GetSafeNormal();
            CurrentCandidate += AdjustmentDirection * StepOffset;
        }
        else if (HitCount > 0 && HitCount < NumCheckPoints)
        {
            break;
        }
        else
        {
            break;
        }
    }
	
    return false;
}
