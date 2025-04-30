// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CommaDash.h"

#include "AbilitySystemComponent.h"
#include "AT/AT_TickCurve.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Prologue/Character/Comma.h"

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
    float CapsuleHalfHeight = Comma->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    FVector FootStartPos = ActorStartPos - FVector(0.f, 0.f, CapsuleHalfHeight);

    FVector InputDirection = MovementComponent->GetLastInputVector();
    InputDirection.Normalize();

    if (InputDirection.IsNearlyZero())
    {
       InputDirection = Comma->GetActorForwardVector();
    }

    FVector TentativeEndFootPos = FootStartPos + InputDirection * MoveLength;
    TArray<AActor*> IgnoreActors;
    IgnoreActors.Add(Comma);
    FHitResult ObstacleHit;

    bool bHitObstacle = UKismetSystemLibrary::LineTraceSingle(
       GetWorld(),
       FootStartPos,
       TentativeEndFootPos,
       UEngineTypes::ConvertToTraceType(ECC_Visibility),
       false,
       IgnoreActors,
       EDrawDebugTrace::ForDuration,
       ObstacleHit,
       true
    );

    FVector CheckedEndFootPos = bHitObstacle ? ObstacleHit.ImpactPoint : TentativeEndFootPos;
    FVector CheckedEndPos = CheckedEndFootPos + FVector(0.f, 0.f, CapsuleHalfHeight);

    BasePos = ActorStartPos;
    FVector LastValidTargetPos = ActorStartPos;
    bool bFoundAnyGroundSpot = false;
    FVector CurrentPathPos;
    FHitResult GroundHit;
	FHitResult VerticalHit;
	
    int32 EffectiveSteps = FMath::Max(1, PathCheckSteps);
    for (int32 i = 1; i <= EffectiveSteps; ++i)
    {
        float Alpha = static_cast<float>(i) / static_cast<float>(EffectiveSteps);
        CurrentPathPos = FMath::Lerp(ActorStartPos, CheckedEndPos, Alpha);

    	bool bFoundTargetThisStep = false;
    	FVector TargetPosThisStep = FVector::ZeroVector;

        FVector GroundTraceStart = CurrentPathPos + FVector(0.f, 0.f, GroundTraceUpOffset);
        FVector GroundTraceEnd = CurrentPathPos - FVector(0.f, 0.f, GroundTraceDistance);

        bool bHitGroundThisStep = UKismetSystemLibrary::LineTraceSingle(
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

        if (bHitGroundThisStep)
        {
			FVector CandidateFootLocation = GroundHit.ImpactPoint;

        	if (IsSafeLandingZone(CandidateFootLocation, IgnoreActors))
        	{
        		LastValidTargetPos = CurrentPathPos;
        		LastValidTargetPos.Z = GroundHit.ImpactPoint.Z + CapsuleHalfHeight + TargetZOffset;
        		bFoundAnyGroundSpot = true;
        	}
        }

    	if (bAllowVerticalDash)
    	{
    		FVector VerticalCheckTop = CurrentPathPos + FVector(0.f, 0.f, MaxAscendHeight);
    		FVector VerticalCheckBottom = VerticalCheckTop - FVector(0.f, 0.f, VerticalCheckTraceLength);

    		bool bHitPlatformAbove = UKismetSystemLibrary::LineTraceSingle(
			   GetWorld(),
			   VerticalCheckTop,
			   VerticalCheckBottom,
			   UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
			   false,
			   IgnoreActors,
			   EDrawDebugTrace::ForDuration,
			   VerticalHit,
			   true,
			   FLinearColor::Green,
			   FLinearColor::Red,
			   1.0f
			);

    		if (bHitPlatformAbove)
    		{
    			if (VerticalHit.ImpactPoint.Z > CurrentPathPos.Z + KINDA_SMALL_NUMBER)
    			{
    				FVector HigherTargetPos = CurrentPathPos;
    				HigherTargetPos.Z = VerticalHit.ImpactPoint.Z + CapsuleHalfHeight + TargetZOffset;

    				TargetPosThisStep = HigherTargetPos;
    				bFoundTargetThisStep = true;
    			}
    		}
    	}

    	if (bFoundTargetThisStep)
    	{
    		LastValidTargetPos = TargetPosThisStep;
    		bFoundAnyGroundSpot = true;
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
        return;
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

bool UGA_CommaDash::IsSafeLandingZone(const FVector& CandidateFootLocation, const TArray<AActor*>& IgnoreActors) const
{
	const float CheckRadius = 30.f;
	const int32 NumCheckPoints = 8;

	for (int32 i = 0; i < NumCheckPoints; ++i)
	{
		float Angle = 2 * PI * (static_cast<float>(i) / NumCheckPoints);
		FVector Offset(FMath::Cos(Angle), FMath::Sin(Angle), 0.f);
		Offset *= CheckRadius;

		FVector Start = CandidateFootLocation + Offset + FVector(0.f, 0.f, GroundTraceUpOffset);
		FVector End = CandidateFootLocation + Offset - FVector(0.f, 0.f, GroundTraceDistance);

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

		if (!bHit)
		{
			return false;
		}
	}

	return true;
}
