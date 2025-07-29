// Copyright Epic Games, Inc. All Rights Reserved.

#include "GA_CommaDash.h"

#include "AbilitySystemComponent.h"
#include "NavigationSystem.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AT/AT_TickCurve.h"
#include "AT/AT_WaitBoolCondition.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/Character/Player/Comma.h"
#include "Prologue/Character/Player/PlayerDashPoint.h"
#include "Prologue/Controller/CommaController.h"

struct FPotentialDashTarget
{
	FVector FeetLocation;
	float DistanceSqToStart;

	FPotentialDashTarget(const FVector& InFeetLocation, const FVector& StartActorPos)
		: FeetLocation(InFeetLocation), DistanceSqToStart(FVector::DistSquared(StartActorPos, InFeetLocation))
	{
	}

	bool operator<(const FPotentialDashTarget& Other) const
	{
		return DistanceSqToStart < Other.DistanceSqToStart;
	}
};


bool UGA_CommaDash::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayTagContainer* SourceTags,
                                       const FGameplayTagContainer* TargetTags,
                                       FGameplayTagContainer* OptionalRelevantTags) const
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
                                    const FGameplayAbilityActivationInfo ActivationInfo,
                                    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AComma* Comma = CastChecked<AComma>(GetAvatarActorFromActorInfo());

	// DashPoint 회전이 끝났다면 즉시 실행
	if (Comma->GetDashPoint()->GetIsDirectionSync())
	{
		LOG_SCREEN("Dash: Sync");
		OnDashAllowed();
	}
	// 아니면 기다렸다 실행
	else
	{
		LOG_SCREEN("Dash: Not Sync");
		auto Condition = [Comma]()
		{
			return Comma
				&& Comma->GetDashPoint()
				&& Comma->GetDashPoint()->GetIsDirectionSync();
		};

		UAT_WaitBoolCondition* WaitTask = UAT_WaitBoolCondition::WaitUntilTrue(this, Condition);
		WaitTask->OnCondition.AddDynamic(this, &UGA_CommaDash::OnDashAllowed);
		WaitTask->ReadyForActivation();
	}

	/*
	// Just Dash Effect 부여
	FGameplayEffectContextHandle JustDashEffectContextHandle = GetAbilitySystemComponentFromActorInfo()->
		MakeEffectContext();
	JustDashEffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle JustDashEffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		JustDashTimingEffect, 0.f, JustDashEffectContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*JustDashEffectSpecHandle.Data.Get());

	// Invincible Effect 부여
	FGameplayEffectContextHandle InvincibleEffectContextHandle = GetAbilitySystemComponentFromActorInfo()->
		MakeEffectContext();
	InvincibleEffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle InvincibleEffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		InvincibleEffect, 0.f, InvincibleEffectContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*InvincibleEffectSpecHandle.Data.Get());

	UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(GetWorld());

	UAbilityTask_PlayMontageAndWait* PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("PlayMontage"), AnimMontage, 1.0f);
	PlayTask->OnCompleted.AddDynamic(this, &UGA_CommaDash::OnComplete);
	PlayTask->OnInterrupted.AddDynamic(this, &UGA_CommaDash::OnInterrupted);
	PlayTask->ReadyForActivation();

	bCanMoveToDashTarget = false;

	if (!Nav)
	{
		return;
	}

	FVector ActorStartPos = Comma->GetActorLocation();
	const float CapsuleHalfHeight = Comma->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float CapsuleRadius = Comma->GetCapsuleComponent()->GetScaledCapsuleRadius();


#pragma region past code
	// // 플레이어의 입력 방향 계산
	// ACommaController* Controller = Cast<ACommaController>(Comma->GetController());
	// FVector DesiredDirection = FVector::ZeroVector;
	//
	// if (Controller)
	// {
	// 	FVector2D CachedMovementInput = Comma->GetCachedMovementInput();
	// 	if (!CachedMovementInput.IsNearlyZero())
	// 	{
	// 		// 카메라 회전을 기준으로 월드 좌표계 방향 계산
	// 		const FRotator ControlRotation = Controller->GetControlRotation();
	// 		const FRotator YawRotation(0, ControlRotation.Yaw, 0);
	// 		const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	// 		const FVector RightDir   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	//
	// 		DesiredDirection = (ForwardDir * CachedMovementInput.Y + RightDir * CachedMovementInput.X).GetSafeNormal();
	// 	}
	// }

	// // 입력이 없을 때는 정면 방향으로 대시
	// if (DesiredDirection.IsNearlyZero())
	// {
	// 	DesiredDirection = Comma->GetActorForwardVector().GetSafeNormal();
	// }
#pragma endregion

	BasePos = ActorStartPos;
	// TargetPos = ActorStartPos;
	TargetPos = Comma->GetDashPoint()->GetPoint();

	float Distance2D = FVector2d::Distance((FVector2d)BasePos, (FVector2d)TargetPos);
	bool bSuccessfullyFoundTarget = Distance2D >= MinDashDistance;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Comma);

#pragma region past code
	// // 경로 검사 단계 수
	// const int32 EffectivePathCheckSteps = FMath::Max(1, PathCheckSteps);
	// // LineTrace 시작 지점 수직 Offset
	// const float LineTraceStartUpOffset = MaxPlatformHeightDiff + 50.f;
	// // LineTrace 종료 지점 수직 Offset
	// const float LineTraceEndDownOffset = MaxPlatformHeightDiff * 2.f + 50.f;

	// // 지정된 방향의 지형 추적 후, 안전한 착지 지점을 검사하는 람다 함수
	// auto PerformTraceInDirection = [&](const FVector& TraceDirection, FVector& OutValidatedFeetPos, EDrawDebugTrace::Type DebugTraceType, UNavigationSystemV1* NavSystem) -> bool
	// {
	// 	FVector CurrentDashAttemptEndPos = ActorStartPos + TraceDirection * MoveLength;
	//
	// 	if (bExtendDashOverActors)
	// 	{
	// 		TArray<FHitResult> HitResults;
	// 		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(DashActorDetection), false);
	// 		QueryParams.AddIgnoredActor(Comma);
	//
	// 		bool bHitActors = GetWorld()->SweepMultiByChannel(
	// 			HitResults,
	// 			ActorStartPos,
	// 			CurrentDashAttemptEndPos,
	// 			FQuat::Identity,
	// 			TraceChannel,
	// 			FCollisionShape::MakeCapsule(CapsuleRadius * 1.5f, CapsuleHalfHeight),
	// 			QueryParams
	// 		);
	//
	// 		if (bHitActors)
	// 		{
	// 			float MaxActorSize = 0.f;
	//
	// 			for (const FHitResult& Hit : HitResults)
	// 			{
	// 				if (APawn* HitPawn = Cast<APawn>(Hit.GetActor()))
	// 				{
	// 					if (UCapsuleComponent* HitCapsule = HitPawn->FindComponentByClass<UCapsuleComponent>())
	// 					{
	// 						float ActorCapsuleRadius = HitCapsule->GetScaledCapsuleRadius();
	// 						MaxActorSize = FMath::Max(MaxActorSize, ActorCapsuleRadius);
	// 					}
	// 				}
	// 			}
	//
	// 			if (MaxActorSize > 0.f)
	// 			{
	// 				float ExtensionDistance = FMath::Min(MaxActorSize * DashExtensionMultiplier, MaxDashExtensionDistance);
	// 				float NewMoveLength = MoveLength + ExtensionDistance;
	//
	// 				CurrentDashAttemptEndPos = ActorStartPos + TraceDirection * NewMoveLength;
	//
	// 				if (bDebugFOVTraces)
	// 				{
	// 					DrawDebugSphere(GetWorld(), CurrentDashAttemptEndPos, 30.f, 12, FColor::Yellow, false, 2.f);
	// 				}
	// 			}
	// 		}
	// 	}
	// 	
	// 	FVector BestValidFeetPosInDirection = ActorStartPos;
	// 	float FarthestValidDistance = 0.f;
	// 	bool bFoundAnyGroundSpotInThisDirection = false;
	//
	// 	// MoveLength를 PathCheckSteps로 나누어 단계 별로 검사
	// 	for (int32 StepIndex = 1; StepIndex <= EffectivePathCheckSteps; ++StepIndex)
	// 	{
	// 		const float Alpha = static_cast<float>(StepIndex) / EffectivePathCheckSteps;
	// 		const FVector SamplePosForTrace = FMath::Lerp(ActorStartPos, CurrentDashAttemptEndPos, Alpha);
	//
	// 		int32 SuccessfulTraces = 0;
	// 		int32 TotalTraces = TracesPerSide * TracesPerSide;
	// 		FVector AccumulatedHitLocation = FVector::ZeroVector;
	//
	// 		const int32 HalfGrid = TracesPerSide / 2;
	// 		
	// 		for(int32 x = -HalfGrid; x <= HalfGrid; ++x)
	// 		{
	// 			for(int32 y = -HalfGrid; y <= HalfGrid; ++y)
	// 			{
	// 				FVector TraceStart = SamplePosForTrace + FVector(x * LineTraceSpread, y * LineTraceSpread, LineTraceStartUpOffset);
	// 				FVector TraceEnd = TraceStart - FVector(0.f, 0.f, LineTraceStartUpOffset + LineTraceEndDownOffset);
	//
	// 				FHitResult GroundHitResult;
	// 				bool bHitGround = UKismetSystemLibrary::LineTraceSingle(
	// 					GetWorld(),
	// 					TraceStart,
	// 					TraceEnd,
	// 					UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
	// 					false,
	// 					ActorsToIgnore,
	// 					DebugTraceType,
	// 					GroundHitResult,
	// 					true,
	// 					FLinearColor::Red, FLinearColor::Green, 1.0f
	// 				);
	//
	// 				if(bHitGround)
	// 				{
	// 					FVector AdjustedPos;
	// 					if (IsSafeLandingZone(GroundHitResult.ImpactPoint, ActorsToIgnore, AdjustedPos))
	// 					{
	// 						SuccessfulTraces++;
	// 						AccumulatedHitLocation += AdjustedPos;
	// 					}
	// 				}
	// 			}
	// 		}
	//
	// 		if (SuccessfulTraces > 0 && (static_cast<float>(SuccessfulTraces) / TotalTraces) >= MinSuccessTracePercentage)
	// 		{
	// 			FVector AverageHitLocation = AccumulatedHitLocation / SuccessfulTraces;
	//
	// 			// NavMesh 검증
	// 			FNavLocation NavMeshLocation;
	// 			const FVector ProjectionExtent(LineTraceSpread * HalfGrid, LineTraceSpread * HalfGrid, MaxPlatformHeightDiff + 100.f);
	// 			if (NavSystem->ProjectPointToNavigation(AverageHitLocation, NavMeshLocation, ProjectionExtent))
	// 			{
	// 				// 현재 위치까지의 거리 계산
	// 				float CurrentDistance = FVector::Dist(ActorStartPos, NavMeshLocation.Location);
	//            
	// 				// 더 멀고 안전한 위치를 발견했을 때만 업데이트
	// 				if (CurrentDistance > FarthestValidDistance)
	// 				{
	// 					FarthestValidDistance = CurrentDistance;
	// 					BestValidFeetPosInDirection = NavMeshLocation.Location;
	// 					bFoundAnyGroundSpotInThisDirection = true;
	// 				}
	// 			}
	// 		}
	// 	}
	//
	// 	if (bFoundAnyGroundSpotInThisDirection)
	// 	{
	// 		if (bAllowPartialDash)
	// 		{
	// 			float AchievedDistance = FVector::Dist(ActorStartPos, BestValidFeetPosInDirection);
	// 			float DesiredDistance = MoveLength;
	// 			float AchievedPercentage = AchievedDistance / DesiredDistance;
	//        
	// 			if (AchievedPercentage >= PartialDashMinPercentage)
	// 			{
	// 				FVector DirectionToStart = (ActorStartPos - BestValidFeetPosInDirection).GetSafeNormal();
	// 				OutValidatedFeetPos = BestValidFeetPosInDirection + DirectionToStart * CapsuleRadius;
	// 				return true;
	// 			}
	// 		}
	// 		else
	// 		{
	// 			FVector DirectionToStart = (ActorStartPos - BestValidFeetPosInDirection).GetSafeNormal();
	// 			OutValidatedFeetPos = BestValidFeetPosInDirection + DirectionToStart * CapsuleRadius;
	// 			return true;
	// 		}
	// 	}
	//
	// 	if (!bFoundAnyGroundSpotInThisDirection)
	// 	{
	// 		const int32 SearchSteps = 8;
	// 		TArray<FVector> PotentialPositions;
	//
	// 		for (int32 i = 0; i < SearchSteps; ++i)
	// 		{
	// 			float Angle = (360.f / SearchSteps) * i;
	// 			FVector SearchOffset = FVector(FMath::Cos(FMath::DegreesToRadians(Angle)) * PlatformEdgeSearchRadius, FMath::Sin(FMath::DegreesToRadians(Angle)) * PlatformEdgeSearchRadius, 0);
	//
	// 			FVector SearchPos = CurrentDashAttemptEndPos + SearchOffset;
	// 			FVector ValidatedPos;
	//
	// 			if (IsSafeLandingZone(SearchPos, ActorsToIgnore, ValidatedPos))
	// 			{
	// 				float Distance = FVector::Dist(ActorStartPos, ValidatedPos);
	//
	// 				if (Distance >= MinDashDistance * 0.5f)
	// 				{
	// 					PotentialPositions.Add(ValidatedPos);
	// 				}
	// 			}
	// 		}
	//
	// 		if (PotentialPositions.Num() > 0)
	// 		{
	// 			float BestDistance = FLT_MAX;
	// 			FVector BestPosition = ActorStartPos;
	//        
	// 			for (const FVector& Pos : PotentialPositions)
	// 			{
	// 				float DistToTarget = FVector::Dist(Pos, CurrentDashAttemptEndPos);
	// 				if (DistToTarget < BestDistance)
	// 				{
	// 					BestDistance = DistToTarget;
	// 					BestPosition = Pos;
	// 				}
	// 			}
	//        
	// 			OutValidatedFeetPos = BestPosition;
	// 			return true;
	// 		}
	// 	}
	//
	// 	return false;
	// };

	// 플레이어가 원하는 방향으로 대시 시도
	// FVector ForwardDashTargetFeetLocation;
	// EDrawDebugTrace::Type PrimaryTraceDebugType = bDebugFOVTraces ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	// if (PerformTraceInDirection(DesiredDirection, ForwardDashTargetFeetLocation, PrimaryTraceDebugType, Nav))
	// {
	// 	TargetPos = ForwardDashTargetFeetLocation;
	// 	bSuccessfullyFoundTarget = true;
	// }
#pragma endregion

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

#pragma region past code
				// FVector FOVTargetFeetLocation;
				// EDrawDebugTrace::Type FOVTraceDebugType = bDebugFOVTraces ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

				// if (PerformTraceInDirection(SampleDirectionForFOV, FOVTargetFeetLocation, FOVTraceDebugType, Nav))
				// {
				// 	PotentialTargetsInFOV.Emplace(FOVTargetFeetLocation, ActorStartPos);
				// }
#pragma  endregion
			}
		}

		if (PotentialTargetsInFOV.Num() > 0)
		{
			PotentialTargetsInFOV.Sort();
			TargetPos = PotentialTargetsInFOV[0].FeetLocation;
			bSuccessfullyFoundTarget = true;
		}
	}

	bCanMoveToDashTarget = false;

	if (bSuccessfullyFoundTarget)
	{
		// FVector FinalValidatedFeetPos;
		TArray<AActor*> FinalCheckIgnoreActors;
		FinalCheckIgnoreActors.Add(Comma);

		TargetPos.Z += Comma->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		if (FVector::DistSquared(BasePos, TargetPos) >= FMath::Square(MinDashDistance))
		{
			bCanMoveToDashTarget = true;
		}

#pragma region past code
		// if (IsSafeLandingZone(TargetPos, FinalCheckIgnoreActors, FinalValidatedFeetPos))
		// {
		// 	TargetPos = FinalValidatedFeetPos;
		// 	TargetPos.Z += Comma->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		//   
		// 	if (FVector::DistSquared(BasePos, TargetPos) >= FMath::Square(MinDashDistance))
		// 	{
		// 		bCanMoveToDashTarget = true;
		// 	}
		// }
#pragma endregion
	}

	if (bCanMoveToDashTarget)
	{
		UAT_TickCurve* TickCurveTask = UAT_TickCurve::CreateTask(this, Curve);
		if (TickCurveTask)
		{
			TickCurveTask->OnCurveTick.AddDynamic(this, &UGA_CommaDash::OnCurveTick);
			TickCurveTask->OnComplete.AddDynamic(this, &UGA_CommaDash::OnComplete);
			TickCurveTask->ReadyForActivation();
		}
	}

#pragma region past code
	// if (!bSuccessfullyFoundTarget)
	// {
	// 	const float FallbackDistances[] = { MoveLength * 0.7f, MoveLength * 0.5f, MoveLength * 0.3f };
	//    
	// 	for (float FallbackDist : FallbackDistances)
	// 	{
	// 		// FVector FallbackDirection = DesiredDirection;
	// 		FVector FallbackDirection = Comma->GetActorLocation() - Comma->GetDashPoint();
	// 		FallbackDirection.Z = 0.0f;
	// 		FallbackDirection.Normalize();
	// 		
	// 		FVector FallbackTarget = ActorStartPos + FallbackDirection * FallbackDist;
	// 		// FVector ValidatedPos;
	//
	// 		// TargetPos = ValidatedPos;
	// 		bSuccessfullyFoundTarget = true;
	// 		break;
	// 		
	// 		// if (IsSafeLandingZone(FallbackTarget, ActorsToIgnore, ValidatedPos))
	// 		// {
	// 		// 	TargetPos = ValidatedPos;
	// 		// 	bSuccessfullyFoundTarget = true;
	// 		// 	break;
	// 		// }
	// 	}
	// }
#pragma  endregion
*/
}


void UGA_CommaDash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                               bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	/* Sejin */
	// 주변 가까운 땅 대시 위치 검사 (대시 보정 알고리즘)
	AComma* Comma = CastChecked<AComma>(GetAvatarActorFromActorInfo());
	Comma->DashPoint->SetDirectionMinGround();
}

void UGA_CommaDash::OnCurveTick(float Alpha)
{
	if (bCanMoveToDashTarget)
	{
		AActor* AvatarActor = GetAvatarActorFromActorInfo();
		if (AvatarActor)
		{
			FVector InterpolatedLocation = FMath::Lerp(BasePos, TargetPos, Alpha);
			AvatarActor->SetActorLocation(InterpolatedLocation);
		}
	}
}

void UGA_CommaDash::OnComplete()
{
	if (bCanMoveToDashTarget)
	{
		AComma* Comma = CastChecked<AComma>(GetAvatarActorFromActorInfo());
		if (Comma && Comma->GetCharacterMovement())
		{
			Comma->GetCharacterMovement()->UpdateFloorFromAdjustment();
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

void UGA_CommaDash::OnAnimationComplete()
{
	bAnimationCompleted = true;
	CheckForAbilityEnd();
}

void UGA_CommaDash::OnCurveComplete()
{
	bCurveCompleted = true;
	CheckForAbilityEnd();
}

bool UGA_CommaDash::IsSafeLandingZone(const FVector& CandidateLocation, const TArray<AActor*>& IgnoreActors,
                                      FVector& OutAdjustedLocation) const
{
	AComma* Comma = CastChecked<AComma>(GetAvatarActorFromActorInfo());
	UCharacterMovementComponent* MovementComp = Comma->GetCharacterMovement();
	UCapsuleComponent* CapsuleComp = Comma->GetCapsuleComponent();

	if (!MovementComp || !CapsuleComp)
	{
		return false;
	}

	const float CapsuleRadius = CapsuleComp->GetScaledCapsuleRadius();
	const float CapsuleHalfHeight = CapsuleComp->GetScaledCapsuleHalfHeight();
	const float MaxStepHeight = MovementComp->MaxStepHeight;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(DashSafetyCheck_Line), false);
	QueryParams.AddIgnoredActors(IgnoreActors);
	QueryParams.AddIgnoredActor(Comma);

	const int32 NumBaseTraces = 8;
	const float TraceAngleIncrement = 360.0f / NumBaseTraces;
	const float TraceUpOffset = MaxStepHeight + 10.f;
	const float TraceDownDistance = MaxPlatformHeightDiff;

	TArray<FVector> HitLocations;
	float MaxZ = -FLT_MAX;
	float MinZ = FLT_MAX;

	for (int32 i = 0; i < NumBaseTraces; ++i)
	{
		const float AngleRad = FMath::DegreesToRadians(i * TraceAngleIncrement);
		const FVector Offset = FVector(FMath::Cos(AngleRad) * CapsuleRadius, FMath::Sin(AngleRad) * CapsuleRadius, 0);
		const FVector TraceStart = CandidateLocation + Offset + FVector(0.f, 0.f, TraceUpOffset);
		const FVector TraceEnd = TraceStart - FVector(0.f, 0.f, TraceDownDistance + TraceUpOffset);

		FHitResult HitResult;
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams);

		if (bDebugFOVTraces)
		{
			DrawDebugLine(GetWorld(), TraceStart, bHit ? HitResult.ImpactPoint : TraceEnd,
			              bHit ? FColor::Cyan : FColor::Magenta, false, 2.0f);
		}

		if (!bHit || !MovementComp->IsWalkable(HitResult))
		{
			return false;
		}

		HitLocations.Add(HitResult.ImpactPoint);
		MaxZ = FMath::Max(MaxZ, HitResult.ImpactPoint.Z);
		MinZ = FMath::Min(MinZ, HitResult.ImpactPoint.Z);
	}

	if ((MaxZ - MinZ) > MaxStepHeight)
	{
		return false;
	}

	const FVector FinalFeetLocation = FVector(CandidateLocation.X, CandidateLocation.Y, MinZ);
	const FVector FinalActorLocation = FinalFeetLocation + FVector(0.f, 0.f, CapsuleHalfHeight);

	const FVector HeadTraceStart = FinalActorLocation;
	const FVector HeadTraceEnd = HeadTraceStart + FVector(0.f, 0.f, CapsuleHalfHeight + 5.f);
	FHitResult HeadHitResult;
	if (GetWorld()->LineTraceSingleByChannel(HeadHitResult, HeadTraceStart, HeadTraceEnd, ECC_WorldStatic, QueryParams))
	{
		if (bDebugFOVTraces)
		{
			DrawDebugLine(GetWorld(), HeadTraceStart, HeadHitResult.ImpactPoint, FColor::Red, false, 2.0f);
		}
		return false;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem)
	{
		FNavLocation NavLocation;
		const FVector ProjectionExtent(CapsuleRadius * 2.f, CapsuleRadius * 2.f, MaxPlatformHeightDiff);
		if (NavSystem->ProjectPointToNavigation(FinalFeetLocation, NavLocation, ProjectionExtent))
		{
			OutAdjustedLocation = FinalFeetLocation;
		}
		else
		{
			return false;
		}
	}
	else
	{
		OutAdjustedLocation = FinalFeetLocation;
	}

	return true;
}

void UGA_CommaDash::OnDashAllowed()
{
	bAnimationCompleted = false;
	bCurveCompleted = false;
	
	LOG_SCREEN("Start Dash");
	AComma* Comma = CastChecked<AComma>(GetAvatarActorFromActorInfo());

	// Just Dash Effect 부여
	FGameplayEffectContextHandle JustDashEffectContextHandle = GetAbilitySystemComponentFromActorInfo()->
		MakeEffectContext();
	JustDashEffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle JustDashEffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		JustDashTimingEffect, 0.f, JustDashEffectContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*JustDashEffectSpecHandle.Data.Get());

	// Invincible Effect 부여
	FGameplayEffectContextHandle InvincibleEffectContextHandle = GetAbilitySystemComponentFromActorInfo()->
		MakeEffectContext();
	InvincibleEffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle InvincibleEffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		InvincibleEffect, 0.f, InvincibleEffectContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*InvincibleEffectSpecHandle.Data.Get());

	UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(GetWorld());

	UAbilityTask_PlayMontageAndWait* PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("PlayMontage"), AnimMontage, 1.0f);
	PlayTask->OnCompleted.AddDynamic(this, &UGA_CommaDash::OnAnimationComplete);
	PlayTask->OnInterrupted.AddDynamic(this, &UGA_CommaDash::OnInterrupted);
	PlayTask->ReadyForActivation();

	bCanMoveToDashTarget = false;

	if (!Nav)
	{
		return;
	}

	FVector ActorStartPos = Comma->GetActorLocation();
	const float CapsuleHalfHeight = Comma->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float CapsuleRadius = Comma->GetCapsuleComponent()->GetScaledCapsuleRadius();

	BasePos = ActorStartPos;
	TargetPos = Comma->GetDashPoint()->GetPoint();

	float Distance2D = FVector2d::Distance((FVector2d)BasePos, (FVector2d)TargetPos);
	bool bSuccessfullyFoundTarget = Distance2D >= MinDashDistance;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Comma);

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
			}
		}

		if (PotentialTargetsInFOV.Num() > 0)
		{
			PotentialTargetsInFOV.Sort();
			TargetPos = PotentialTargetsInFOV[0].FeetLocation;
			bSuccessfullyFoundTarget = true;
		}
	}

	bCanMoveToDashTarget = false;

	if (bSuccessfullyFoundTarget)
	{
		TArray<AActor*> FinalCheckIgnoreActors;
		FinalCheckIgnoreActors.Add(Comma);

		TargetPos.Z += Comma->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		if (FVector::DistSquared(BasePos, TargetPos) >= FMath::Square(MinDashDistance))
		{
			bCanMoveToDashTarget = true;
		}
	}

	if (bCanMoveToDashTarget)
	{
		UAT_TickCurve* TickCurveTask = UAT_TickCurve::CreateTask(this, Curve);
		if (TickCurveTask)
		{
			TickCurveTask->OnCurveTick.AddDynamic(this, &UGA_CommaDash::OnCurveTick);
			TickCurveTask->OnComplete.AddDynamic(this, &UGA_CommaDash::OnCurveComplete);
			TickCurveTask->ReadyForActivation();
		}
	}
	else
	{
		bCurveCompleted = true;
	}
}

void UGA_CommaDash::CheckForAbilityEnd()
{
	if (bAnimationCompleted && bCurveCompleted)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
