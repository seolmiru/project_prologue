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

UGA_CommaDash::UGA_CommaDash()
{
	// ActivationBlockedTags.AddTag(CooldownTag);
	// UE_LOG(LogTemp, Log, TEXT("Cooldown: %s"), *CooldownTag.ToString());
}

bool UGA_CommaDash::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayTagContainer* SourceTags,
                                       const FGameplayTagContainer* TargetTags,
                                       FGameplayTagContainer* OptionalRelevantTags) const
{
	AComma* Comma = CastChecked<AComma>(GetAvatarActorFromActorInfo());
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
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
}


void UGA_CommaDash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                               bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	// 이동 속도 증가 이펙트 적용
	FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle EffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(SpeedBoostEffect, 0.f, EffectContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	LOG_SCREEN_R("Dash Speed Boost");
	
	/* Sejin */
	// 주변 가까운 땅 대시 위치 검사 (대시 보정 알고리즘)
	AComma* Comma = CastChecked<AComma>(GetAvatarActorFromActorInfo());
	Comma->GetDashPoint()->SetDirectionMinGround();

	if (Comma->GetInputDashState())
	{
		Comma->InputGAS(FGameplayTag::RequestGameplayTag("Comma.Ability.Dash"));
	}
}

void UGA_CommaDash::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

UGameplayEffect* UGA_CommaDash::GetCooldownGameplayEffect() const
{
	AComma* Comma = CastChecked<AComma>(GetAvatarActorFromActorInfo());
	if (Comma->GetDashPoint()->GetDashCoolState())
	{
		LOG_SCREEN("Dash: Long Cool");
		return CoolEffectLong.GetDefaultObject();
	}
	else
	{
		LOG_SCREEN("Dash: Short Cool");
		return CoolEffectShort.GetDefaultObject();
	}
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
	AComma* Comma = CastChecked<AComma>(GetAvatarActorFromActorInfo());
	Comma->GetDashPoint()->GetDashCoolState(); // 대시 쿨타임 설정
	CommitAbilityCooldown(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);

	// if (Comma->GetDashPoint()->GetDashCoolState())
	// {
	// }

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
			TickCurveTask->ReadyForActivation();
		}
	}
}
