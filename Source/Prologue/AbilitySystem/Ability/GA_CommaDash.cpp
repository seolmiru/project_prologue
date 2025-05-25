// Copyright Epic Games, Inc. All Rights Reserved.

#include "GA_CommaDash.h"

#include "AbilitySystemComponent.h"
#include "NavigationSystem.h"
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

	AComma* Comma = CastChecked<AComma>(GetAvatarActorFromActorInfo());
	UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(GetWorld());

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

	ACommaController* Controller = Cast<ACommaController>(Comma->GetController());
	FVector DesiredDirection = FVector::ZeroVector;

	if (Controller)
	{
		FVector2D CachedMovementInput = Comma->GetCachedMovementInput();
		if (!CachedMovementInput.IsNearlyZero())
		{
			const FRotator ControlRotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, ControlRotation.Yaw, 0);
			const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			const FVector RightDir   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			DesiredDirection = (ForwardDir * CachedMovementInput.Y + RightDir * CachedMovementInput.X).GetSafeNormal();
		}
	}
	
	if (DesiredDirection.IsNearlyZero())
	{
		DesiredDirection = Comma->GetActorForwardVector().GetSafeNormal();
	}

	BasePos = ActorStartPos;
	TargetPos = ActorStartPos;
	bool bSuccessfullyFoundTarget = false;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Comma);

	const int32 EffectivePathCheckSteps = FMath::Max(1, PathCheckSteps);
	const float SphereTraceStartUpOffset = MaxPlatformHeightDiff + 50.f;
	const float SphereTraceEndDownOffset = MaxPlatformHeightDiff * 2.f + 50.f;

	auto PerformTraceInDirection = [&](const FVector& TraceDirection, FVector& OutValidatedFeetPos, EDrawDebugTrace::Type DebugTraceType, UNavigationSystemV1* NavSystem) -> bool
	{
		FVector CurrentDashAttemptEndPos = ActorStartPos + TraceDirection * MoveLength;
		FVector BestValidFeetPosInDirection = ActorStartPos;
		bool bFoundAnyGroundSpotInThisDirection = false;

		for (int32 StepIndex = 1; StepIndex <= EffectivePathCheckSteps; ++StepIndex)
		{
			const float Alpha = static_cast<float>(StepIndex) / EffectivePathCheckSteps;
			const FVector SamplePosForTrace = FMath::Lerp(ActorStartPos, CurrentDashAttemptEndPos, Alpha);

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
			
			if (!bHitGround) continue;

			FVector GroundImpactPoint = GroundHitResult.ImpactPoint;
			FVector AdjustedSafeFeetPosOnGround;

			if (!IsSafeLandingZone(GroundImpactPoint, ActorsToIgnore, AdjustedSafeFeetPosOnGround)) continue;

			if (!NavSystem)
			{
				continue;
			}

			FNavLocation NavMeshLocation;

			const FVector ProjectionExtent(GroundTraceRadius, GroundTraceRadius, MaxPlatformHeightDiff + 100.f);

			if (!NavSystem->ProjectPointToNavigation(AdjustedSafeFeetPosOnGround, NavMeshLocation, ProjectionExtent))
			{
				continue;				
			}

			AdjustedSafeFeetPosOnGround = NavMeshLocation.Location;
			BestValidFeetPosInDirection = AdjustedSafeFeetPosOnGround;
			bFoundAnyGroundSpotInThisDirection = true;
		}

		if (bFoundAnyGroundSpotInThisDirection)
		{
			OutValidatedFeetPos = BestValidFeetPosInDirection;

			return true;
		}

		return false;
	};

	FVector ForwardDashTargetFeetLocation;
	EDrawDebugTrace::Type PrimaryTraceDebugType = bDebugFOVTraces ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	if (PerformTraceInDirection(DesiredDirection, ForwardDashTargetFeetLocation, PrimaryTraceDebugType, Nav))
	{
		TargetPos = ForwardDashTargetFeetLocation;
		bSuccessfullyFoundTarget = true;
	}

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

		if (FVector::DistSquared(BasePos, TargetPos) < FMath::Square(MinDashDistance))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
		
		TickCurveTask->ReadyForActivation();
	}
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
		AvatarActor->SetActorLocation(FMath::Lerp(BasePos, TargetPos, Alpha));
	}
}

void UGA_CommaDash::OnComplete()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

bool UGA_CommaDash::IsSafeLandingZone(const FVector& CandidateLocation, const TArray<AActor*>& IgnoreActors, FVector& OutAdjustedLocation) const
{
    const int32 MaxIterations = 3;
    const float StepOffset = 20.f;
    const FVector UpDownOffsetForCheck(0.f, 0.f, MaxPlatformHeightDiff);
    const float CheckRadius = 30.f;
    const int32 NumCheckPoints = 8;
    const float MinValidHitRatio = 0.75f;

    FVector CurrentCandidate = CandidateLocation;

    for (int32 Iter = 0; Iter < MaxIterations; ++Iter)
    {
        int32 HitCount = 0;
        FVector AccumulatedNonHitOffset = FVector::ZeroVector;

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

        if (HitCount < NumCheckPoints / 2 && Iter > 0)
        {
            break;
        }

        if (!AccumulatedNonHitOffset.IsNearlyZero())
        {
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
