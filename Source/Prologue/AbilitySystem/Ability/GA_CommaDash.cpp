// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CommaDash.h"

#include "AbilitySystemComponent.h"
#include "AT/AT_TickCurve.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Prologue/Character/Player/Comma.h"

void UGA_CommaDash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AComma* Comma = CastChecked<AComma>(GetAvatarActorFromActorInfo());
	UCharacterMovementComponent* MovementComponent = Comma->GetCharacterMovement();

	UAT_TickCurve* TickCurve = UAT_TickCurve::CreateTask(this, Curve);
	TickCurve->OnCurveTick.AddDynamic(this, &UGA_CommaDash::OnCurveTick);
	TickCurve->OnComplete.AddDynamic(this, &UGA_CommaDash::OnComplete);

	FVector ActorStartPos = Comma->GetActorLocation();
	const float CapsuleHalfHeight = Comma->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	FVector InputDirection = MovementComponent->GetLastInputVector();
	InputDirection.Normalize();
	if (InputDirection.IsNearlyZero())
	{
		InputDirection = Comma->GetActorForwardVector();
	}

	const FVector DashEndPos = ActorStartPos + InputDirection * MoveLength;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Comma);

	BasePos = ActorStartPos;
	FVector LastValidTargetPos = ActorStartPos;
	bool bFoundAnyGroundSpot = false;

	const int32 EffectiveSteps = FMath::Max(1, PathCheckSteps);
	for (int32 i = 1; i <= EffectiveSteps; ++i)
	{
		const float Alpha = static_cast<float>(i) / static_cast<float>(EffectiveSteps);
		const FVector SamplePos = FMath::Lerp(ActorStartPos, DashEndPos, Alpha);

		const FVector GroundTraceStart = SamplePos + FVector(0.f, 0.f, GroundTraceUpOffset);
		const FVector GroundTraceEnd = SamplePos - FVector(0.f, 0.f, GroundTraceDistance);

		FHitResult GroundHit;
		const bool bHitGround = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			GroundTraceStart,
			GroundTraceEnd,
			UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
			false,
			IgnoreActors,
			EDrawDebugTrace::ForDuration,
			GroundHit,
			true,
			FLinearColor::Green,
			FLinearColor::Red,
			1.0f
		);

		if (bHitGround)
		{
			FVector AdjustedLandingPos;
			if (IsSafeLandingZone(GroundHit.ImpactPoint, IgnoreActors, AdjustedLandingPos))
			{
				LastValidTargetPos = AdjustedLandingPos + FVector(0.f, 0.f, CapsuleHalfHeight + TargetZOffset);
				bFoundAnyGroundSpot = true;
			}
		}
	}

	if (bFoundAnyGroundSpot)
	{
		TargetPos = LastValidTargetPos;

		if (FVector::DistSquared(BasePos, TargetPos) < MinDashDistance * MinDashDistance)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			return;
		}

		TickCurve->ReadyForActivation();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UGA_CommaDash::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
}

void UGA_CommaDash::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_CommaDash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_CommaDash::OnCurveTick(float Alpha)
{
	GetAvatarActorFromActorInfo()->SetActorLocation(FMath::Lerp(BasePos, TargetPos, Alpha));
}

void UGA_CommaDash::OnComplete()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

bool UGA_CommaDash::IsSafeLandingZone(const FVector& CandidateLocation, const TArray<AActor*>& IgnoreActors, FVector& OutAdjustedLocation) const
{
	const float CheckRadius = 30.f;
	const int32 NumCheckPoints = 8;
	const float MinValidHitRatio = 0.75f;

	int32 HitCount = 0;
	FVector AccumulatedOffset = FVector::ZeroVector;

	for (int32 i = 0; i < NumCheckPoints; ++i)
	{
		float Angle = 2 * PI * (static_cast<float>(i) / NumCheckPoints);
		FVector Offset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * CheckRadius;

		FVector Start = CandidateLocation + Offset + FVector(0.f, 0.f, GroundTraceUpOffset);
		FVector End = CandidateLocation + Offset - FVector(0.f, 0.f, GroundTraceDistance);

		FHitResult Hit;
		bool bHit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			Start,
			End,
			UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
			false,
			IgnoreActors,
			EDrawDebugTrace::None,
			Hit,
			true
		);

		if (bHit)
		{
			HitCount++;
			AccumulatedOffset -= Offset;
		}
	}

	float HitRatio = static_cast<float>(HitCount) / static_cast<float>(NumCheckPoints);

	if (HitRatio >= MinValidHitRatio)
	{
		OutAdjustedLocation = CandidateLocation;
		return true;
	}
	else if (HitCount >= 4)
	{
		const FVector Adjusted = CandidateLocation + AccumulatedOffset.GetSafeNormal() * 20.f;
		if (bool bRecursiveResult = IsSafeLandingZone(Adjusted, IgnoreActors, OutAdjustedLocation))
			return true;
	}
	
	return false;
}
