// Copyright Epic Games, Inc. All Rights Reserved.

#include "GA_CommaDash.h"

#include "AbilitySystemComponent.h"
#include "NavigationSystem.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AT/AT_TickCurve.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Prologue/PrologueGameplayTags.h"
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


bool UGA_CommaDash::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (!bIgnoreCancelRestriction)
	{
		if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(PrologueGameplayTags::Comma_State_CancelDisabled))
		{
			return false;
		}
	}

	return true;
}

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
	const float CapsuleRadius = Comma->GetCapsuleComponent()->GetScaledCapsuleRadius();

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
	// LineTrace 시작 지점 수직 Offset
	const float LineTraceStartUpOffset = MaxPlatformHeightDiff + 50.f;
	// LineTrace 종료 지점 수직 Offset
	const float LineTraceEndDownOffset = MaxPlatformHeightDiff * 2.f + 50.f;

	// 지정된 방향의 지형 추적 후, 안전한 착지 지점을 검사하는 람다 함수
	auto PerformTraceInDirection = [&](const FVector& TraceDirection, FVector& OutValidatedFeetPos, EDrawDebugTrace::Type DebugTraceType, UNavigationSystemV1* NavSystem) -> bool
	{
		FVector CurrentDashAttemptEndPos = ActorStartPos + TraceDirection * MoveLength;
		FVector BestValidFeetPosInDirection = ActorStartPos;
		float FarthestValidDistance = 0.f;
		bool bFoundAnyGroundSpotInThisDirection = false;

		// MoveLength를 PathCheckSteps로 나누어 단계 별로 검사
		for (int32 StepIndex = 1; StepIndex <= EffectivePathCheckSteps; ++StepIndex)
		{
			const float Alpha = static_cast<float>(StepIndex) / EffectivePathCheckSteps;
			const FVector SamplePosForTrace = FMath::Lerp(ActorStartPos, CurrentDashAttemptEndPos, Alpha);

			int32 SuccessfulTraces = 0;
			int32 TotalTraces = TracesPerSide * TracesPerSide;
			FVector AccumulatedHitLocation = FVector::ZeroVector;

			const int32 HalfGrid = TracesPerSide / 2;
			
			for(int32 x = -HalfGrid; x <= HalfGrid; ++x)
			{
				for(int32 y = -HalfGrid; y <= HalfGrid; ++y)
				{
					FVector TraceStart = SamplePosForTrace + FVector(x * LineTraceSpread, y * LineTraceSpread, LineTraceStartUpOffset);
					FVector TraceEnd = TraceStart - FVector(0.f, 0.f, LineTraceStartUpOffset + LineTraceEndDownOffset);

					FHitResult GroundHitResult;
					bool bHitGround = UKismetSystemLibrary::LineTraceSingle(
						GetWorld(),
						TraceStart,
						TraceEnd,
						UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
						false,
						ActorsToIgnore,
						DebugTraceType,
						GroundHitResult,
						true,
						FLinearColor::Red, FLinearColor::Green, 1.0f
					);

					if(bHitGround)
					{
						FVector AdjustedPos;
						if (IsSafeLandingZone(GroundHitResult.ImpactPoint, ActorsToIgnore, AdjustedPos))
						{
							SuccessfulTraces++;
							AccumulatedHitLocation += AdjustedPos;
						}
					}
				}
			}

			if (SuccessfulTraces > 0 && (static_cast<float>(SuccessfulTraces) / TotalTraces) >= MinSuccessTracePercentage)
			{
				FVector AverageHitLocation = AccumulatedHitLocation / SuccessfulTraces;

				// NavMesh 검증
				FNavLocation NavMeshLocation;
				const FVector ProjectionExtent(LineTraceSpread * HalfGrid, LineTraceSpread * HalfGrid, MaxPlatformHeightDiff + 100.f);
				if (NavSystem->ProjectPointToNavigation(AverageHitLocation, NavMeshLocation, ProjectionExtent))
				{
					AverageHitLocation = NavMeshLocation.Location;
					
					// 현재 위치까지의 거리 계산
					float CurrentDistance = FVector::Dist(ActorStartPos, AverageHitLocation);
            
					// 더 멀고 안전한 위치를 발견했을 때만 업데이트
					if (CurrentDistance > FarthestValidDistance)
					{
						FarthestValidDistance = CurrentDistance;
						BestValidFeetPosInDirection = AverageHitLocation;
						bFoundAnyGroundSpotInThisDirection = true;
					}
				}
			}
		}

		if (bFoundAnyGroundSpotInThisDirection)
		{
			if (bAllowPartialDash)
			{
				float AchievedDistance = FVector::Dist(ActorStartPos, BestValidFeetPosInDirection);
				float DesiredDistance = MoveLength;
				float AchievedPercentage = AchievedDistance / DesiredDistance;
        
				if (AchievedPercentage >= PartialDashMinPercentage)
				{
					FVector DirectionToStart = (ActorStartPos - BestValidFeetPosInDirection).GetSafeNormal();
					OutValidatedFeetPos = BestValidFeetPosInDirection + DirectionToStart * CapsuleRadius;
					return true;
				}
			}
			else
			{
				FVector DirectionToStart = (ActorStartPos - BestValidFeetPosInDirection).GetSafeNormal();
				OutValidatedFeetPos = BestValidFeetPosInDirection + DirectionToStart * CapsuleRadius;
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
		TargetPos.Z += CapsuleHalfHeight;
    
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

// 여러 개의 LineTrace를 사용하여 안전한 착지 위치인지 검증하는 함수
bool UGA_CommaDash::IsSafeLandingZone(const FVector& CandidateLocation, const TArray<AActor*>& IgnoreActors, FVector& OutAdjustedLocation) const
{
	AComma* Comma = CastChecked<AComma>(GetAvatarActorFromActorInfo());
	if (!Comma || !Comma->GetCharacterMovement()) return false;

	const float CapsuleRadius = Comma->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const float TraceUpOffset = 50.f;
	const float TraceDownDistance = MaxPlatformHeightDiff;

	// 중앙 및 주변 네 방향에 대한 오프셋 정의
	TArray<FVector> Offsets;
	Offsets.Add(FVector::ZeroVector); // Center
	Offsets.Add(FVector(CapsuleRadius, 0.f, 0.f)); // Forward
	Offsets.Add(FVector(-CapsuleRadius, 0.f, 0.f)); // Backward
	Offsets.Add(FVector(0.f, CapsuleRadius, 0.f)); // Right
	Offsets.Add(FVector(0.f, -CapsuleRadius, 0.f)); // Left

	FVector AccumulatedImpactPoint = FVector::ZeroVector;
	int32 HitCount = 0;

	for (const FVector& Offset : Offsets)
	{
		FHitResult HitResult;
		const FVector Start = CandidateLocation + Offset + FVector(0.f, 0.f, TraceUpOffset);
		const FVector End = CandidateLocation + Offset - FVector(0.f, 0.f, TraceDownDistance);
		
		const bool bHit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			Start,
			End,
			UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
			false,
			IgnoreActors,
			bDebugFOVTraces ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
			HitResult,
			true
		);

		// 트레이스가 성공하고, 걸을 수 있는 표면이어야 함
		if (bHit && Comma->GetCharacterMovement()->IsWalkable(HitResult))
		{
			AccumulatedImpactPoint += HitResult.ImpactPoint;
			HitCount++;
		}
		else
		{
			return false;
		}
	}

	// 모든 트레이스가 성공했을 경우, 평균 충돌 지점을 계산
	if (HitCount == Offsets.Num())
	{
		FVector AverageImpactPoint = AccumulatedImpactPoint / HitCount;
		OutAdjustedLocation = FVector(CandidateLocation.X, CandidateLocation.Y, AverageImpactPoint.Z);
		return true;
	}

	return false;
}