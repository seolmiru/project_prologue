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
#include "Engine/OverlapResult.h"

// 제발 좀 되라

struct FPotentialDashTarget
{
    FVector FeetLocation;
    float DistanceSqToStart;

    FPotentialDashTarget(const FVector& InFeetLocation, const FVector& StartActorFeetPos)
        : FeetLocation(InFeetLocation), DistanceSqToStart(FVector::DistSquared(StartActorFeetPos, InFeetLocation)) {}

    bool operator<(const FPotentialDashTarget& Other) const
    {
        return DistanceSqToStart < Other.DistanceSqToStart;
    }
};


void UGA_CommaDash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Just Dash Timing Effect
	FGameplayEffectContextHandle JustDashEffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	JustDashEffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle JustDashEffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(JustDashTimingEffect, 0.f, JustDashEffectContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*JustDashEffectSpecHandle.Data.Get());

	// Invincible Effect
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
    const FVector ActorStartFeetPos = ActorStartPos - FVector(0.f, 0.f, CapsuleHalfHeight);

	// 플레이어 입력 방향 계산
	ACommaController* Controller = Cast<ACommaController>(Comma->GetController());
	FVector DesiredDirection = FVector::ZeroVector;

	// 방향 입력이 들어왔을 때 카메라 기준으로 월드 좌표계로 방향 계산
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

	// 방향 입력이 없으면 캐릭터의 전방으로 대시 실행
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

	// 플레이어의 대시 방향으로 여러 플랫폼을 검사하고 그중 안전한 착지 지점을 결정하는 람다 함수
	// 람다 함수 개선 필요
	auto PerformTraceInDirection = [&](const FVector& TraceDirection, FVector& OutValidatedFeetPos, EDrawDebugTrace::Type DebugTraceType, UNavigationSystemV1* NavSystem) -> bool
	{
		// 대시 최종 도착 지점 계산
		FVector CurrentDashAttemptEndPosFeet = ActorStartFeetPos + TraceDirection * MoveLength;
		FVector BestValidFeetPosInDirection = ActorStartFeetPos;
		float FarthestValidDistance = 0.f;
		bool bFoundAnyGroundSpotInThisDirection = false;

		// MoveLength 거리를 PathCheckSteps로 나누어서 단계 별로 검사
		for (int32 StepIndex = 1; StepIndex <= EffectivePathCheckSteps; ++StepIndex)
		{
			const float Alpha = static_cast<float>(StepIndex) / EffectivePathCheckSteps;
			const FVector SamplePosForTraceFeet = FMath::Lerp(ActorStartFeetPos, CurrentDashAttemptEndPosFeet, Alpha);

			// SphereTrace로 플랫폼 검사
			FHitResult GroundHitResult;
			const bool bHitGround = UKismetSystemLibrary::SphereTraceSingle(
				GetWorld(),
				SamplePosForTraceFeet + FVector(0.f, 0.f, SphereTraceStartUpOffset),
				SamplePosForTraceFeet - FVector(0.f, 0.f, SphereTraceEndDownOffset),
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
				continue;
			}

			FVector GroundImpactPointFeet = GroundHitResult.ImpactPoint;
			FVector AdjustedSafeFeetPosOnGround;

			if (!IsSafeLandingZone(GroundImpactPointFeet, ActorsToIgnore, AdjustedSafeFeetPosOnGround)) 
			{
				continue;
			}
        
			float CurrentDistance = FVector::DistSquared(ActorStartFeetPos, AdjustedSafeFeetPosOnGround);

			// 가장 멀리 있으면서 안전한 위치가 있다면 대시 가능 지점 업데이트
			if (CurrentDistance > FMath::Square(FarthestValidDistance))
			{
				FarthestValidDistance = FMath::Sqrt(CurrentDistance);
				BestValidFeetPosInDirection = AdjustedSafeFeetPosOnGround;
				bFoundAnyGroundSpotInThisDirection = true;
			}
		}

		if (bFoundAnyGroundSpotInThisDirection)
		{
			// 부분 대시 허용 시에 사용되는 거리 체크
			if (bAllowPartialDash)
			{
				float AchievedDistance = FVector::Dist(ActorStartFeetPos, BestValidFeetPosInDirection);
				float DesiredDistance = MoveLength;
				float AchievedPercentage = AchievedDistance / DesiredDistance;
        
				if (AchievedPercentage >= PartialDashMinPercentage)
				{
					OutValidatedFeetPos = BestValidFeetPosInDirection;
					return true;
				}
			}
			// 부분 대시를 사용하지 않을 때에는 발견한 위치를 그대로 적용
			else
			{
				OutValidatedFeetPos = BestValidFeetPosInDirection;
				return true;
			}
		}
		
		return false;
	};

	// 플레이어가 원하는 방향으로 대시 시도
	FVector ForwardDashTargetFeetLocation;
	EDrawDebugTrace::Type PrimaryTraceDebugType = bDebugTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	if (PerformTraceInDirection(DesiredDirection, ForwardDashTargetFeetLocation, PrimaryTraceDebugType, Nav))
	{
		TargetPos = ForwardDashTargetFeetLocation + FVector(0.f, 0.f, CapsuleHalfHeight + TargetZOffset);
		bSuccessfullyFoundTarget = true;
	}

	// 직선 방향에 땅이 없거나 무언가로 막혀있다면 FOV 범위 내에 NumFOVTracePerSide만큼 추가적으로 주변 경로 탐색
	// 플레이어가 정확한 방향으로 플랫폼을 바라보고 있지 않더라도 어느정도 범위 내에 플랫폼이 있다면 대시가 가능하도록 해줌
	if (!bSuccessfullyFoundTarget && NumFOVTracesPerSide > 0 && FOVAngleDegrees > 0.f)
	{
		TArray<FPotentialDashTarget> PotentialTargetsInFOV;
		FVector CharacterActualForwardDir = Comma->GetActorForwardVector().GetSafeNormal();
        FVector FOVBaseDirection = DesiredDirection.IsNearlyZero() ? CharacterActualForwardDir : DesiredDirection;


		float AngleIncrementPerTrace = (FOVAngleDegrees / 2.0f) / FMath::Max(1, NumFOVTracesPerSide);

		for (int32 Side = -1; Side <= 1; Side += 2)
		{
            if (Side == 0 && NumFOVTracesPerSide > 0) continue;

			for (int32 TraceIndex = 1; TraceIndex <= NumFOVTracesPerSide; ++TraceIndex)
			{
				float CurrentSampleAngleDegrees = AngleIncrementPerTrace * TraceIndex * Side;
				FRotator AngleOffsetRotator(0, CurrentSampleAngleDegrees, 0);
				FVector SampleDirectionForFOV = AngleOffsetRotator.RotateVector(FOVBaseDirection);
				
				FVector FOVTargetFeetLocation;
				EDrawDebugTrace::Type FOVTraceDebugType = bDebugTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

				if (PerformTraceInDirection(SampleDirectionForFOV, FOVTargetFeetLocation, FOVTraceDebugType, Nav))
				{
					PotentialTargetsInFOV.Emplace(FOVTargetFeetLocation, ActorStartFeetPos);
				}
			}
		}

		// FOV 검사 중 안전한 위치를 발견했다면 안전한 위치들 중에서 가장 가까운 곳을 최종 목적지로 변경
		if (PotentialTargetsInFOV.Num() > 0)
		{
			PotentialTargetsInFOV.Sort();
			TargetPos = PotentialTargetsInFOV[0].FeetLocation + FVector(0.f, 0.f, CapsuleHalfHeight + TargetZOffset);
			bSuccessfullyFoundTarget = true;
		}
	}

	// 최종 검증
	if (bSuccessfullyFoundTarget)
	{
		if (FVector::DistSquared(BasePos, TargetPos) < FMath::Square(MinDashDistance))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
		
		TickCurveTask->ReadyForActivation();
	}
	// 모든 검사가 실패하고 안전한 땅이 없다면 EndAbility 호출
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

// MovementComponent를 가져와서 최종적으로 플레이어가 안전한 위치에 있는지, 정확하게 플랫폼 위에 서있을 수 있는지 검사
void UGA_CommaDash::OnComplete()
{
    AComma* Comma = CastChecked<AComma>(GetAvatarActorFromActorInfo());
    if (Comma)
    {
        UCharacterMovementComponent* MovementComp = Comma->GetCharacterMovement();
        if (MovementComp)
        {
            MovementComp->UpdateFloorFromAdjustment();

        	// 최종 목적지에 바닥이 없거나 불안정하다면 근처 가까운 위치로 조정
            if (!MovementComp->IsMovingOnGround())
            {
                FVector CurrentCharacterLocation = Comma->GetActorLocation();
                const float CapsuleHalfHeight = Comma->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
                FVector CurrentFeetLocation = CurrentCharacterLocation - FVector(0.f, 0.f, CapsuleHalfHeight);

                TArray<AActor*> IgnoreActors;
                IgnoreActors.Add(Comma);
                
                FVector AdjustedSafeFeetLocation;
                if (IsSafeLandingZone(CurrentFeetLocation, IgnoreActors, AdjustedSafeFeetLocation))
                {
                    Comma->SetActorLocation(AdjustedSafeFeetLocation + FVector(0.f, 0.f, CapsuleHalfHeight));
					if (bDebugTrace)
					{
						LOG_SCREEN("UGA_CommaDash::OnComplete - Adjusted to safe landing spot: %s", *(AdjustedSafeFeetLocation + FVector(0.f, 0.f, CapsuleHalfHeight)).ToString());
					}
                }
				else
				{
					if (bDebugTrace)
					{
						LOG_SCREEN("UGA_CommaDash::OnComplete - Could not find safe landing spot after dash. Current Feet: %s", *CurrentFeetLocation.ToString());
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

// 대시 위치가 안전한지 검증하는 함수
bool UGA_CommaDash::IsSafeLandingZone(const FVector& CandidateFeetLocation, const TArray<AActor*>& IgnoreActors, FVector& OutAdjustedFeetLocation) const
{
    AComma* Comma = CastChecked<AComma>(GetAvatarActorFromActorInfo());
    if (!Comma || !GetWorld()) return false;

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys)
	{
		return false;
	}

    const float CapsuleRadius = Comma->GetCapsuleComponent()->GetScaledCapsuleRadius();
    const float CapsuleHalfHeight = Comma->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

    FNavLocation ProjectedNavLocation;
    const FVector ProjectionExtent(CapsuleRadius * 0.5f, CapsuleRadius * 0.5f, MaxPlatformHeightDiff * 0.5f); 

    if (!NavSys->ProjectPointToNavigation(CandidateFeetLocation, ProjectedNavLocation, ProjectionExtent))
    {
        if (bDebugTrace)
        {
            DrawDebugSphere(GetWorld(), CandidateFeetLocation, CapsuleRadius * 0.5f, 12, FColor::Red, false, 2.0f, 0, 1.f);
        }
    	
        return false;
    }

    FVector BaseNavMeshFeetLocation = ProjectedNavLocation.Location;

    TArray<FVector> SamplePointsRelative;
    SamplePointsRelative.Add(FVector::ZeroVector);
    SamplePointsRelative.Add(FVector(CapsuleRadius * 0.8f, 0.f, 0.f));
    SamplePointsRelative.Add(FVector(-CapsuleRadius * 0.8f, 0.f, 0.f));
    SamplePointsRelative.Add(FVector(0.f, CapsuleRadius * 0.8f, 0.f));
    SamplePointsRelative.Add(FVector(0.f, -CapsuleRadius * 0.8f, 0.f));


    for (const FVector& RelativePoint : SamplePointsRelative)
    {
        FVector WorldSamplePoint = BaseNavMeshFeetLocation + RelativePoint;
        FNavLocation SampleNavLocation;
        if (!NavSys->ProjectPointToNavigation(WorldSamplePoint, SampleNavLocation, FVector(10.f, 10.f, MaxStepHeightForValidGround)))
        {
            if (bDebugTrace)
            {
                DrawDebugSphere(GetWorld(), WorldSamplePoint, 5.f, 12, FColor::Yellow, false, 2.0f, 0, 1.f);
            }
        	
            return false;
        }
        if (FMath::Abs(SampleNavLocation.Location.Z - BaseNavMeshFeetLocation.Z) > MaxStepHeightForValidGround) 
        {
             if (bDebugTrace)
            {
                DrawDebugLine(GetWorld(), BaseNavMeshFeetLocation, SampleNavLocation.Location, FColor::Orange, false, 2.f, 0, 1.f);
            }
        	
            return false;
        }
    }

    FVector CharacterCapsuleCenterAtLanding = BaseNavMeshFeetLocation + FVector(0.f, 0.f, CapsuleHalfHeight);

    TArray<FOverlapResult> OverlapResults;
    FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleRadius * 0.95f, CapsuleHalfHeight * 0.95f);
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActors(IgnoreActors);
	QueryParams.bTraceComplex = false;

    bool bOverlap = GetWorld()->OverlapMultiByObjectType(
        OverlapResults,
        CharacterCapsuleCenterAtLanding,
        FQuat::Identity,
        FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
        CapsuleShape,
        QueryParams
    );

    if (bOverlap)
    {
        for (const FOverlapResult& Result : OverlapResults)
        {
            if (Result.GetActor() && Result.GetComponent() && Result.GetComponent()->IsCollisionEnabled() && Result.GetComponent()->GetCollisionResponseToChannel(ECC_Pawn) == ECR_Block)
            {
                if (bDebugTrace)
                {
                    DrawDebugCapsule(GetWorld(), CharacterCapsuleCenterAtLanding, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity, FColor::Magenta, false, 2.0f, 0, 2.f);
                }
            	
                return false; 
            }
        }
    }

    OutAdjustedFeetLocation = BaseNavMeshFeetLocation; 
    if (bDebugTrace)
    {
        DrawDebugSphere(GetWorld(), OutAdjustedFeetLocation, CapsuleRadius * 0.5f, 12, FColor::Green, false, 2.0f, 0, 1.f);
        DrawDebugCapsule(GetWorld(), OutAdjustedFeetLocation + FVector(0.f, 0.f, CapsuleHalfHeight), CapsuleHalfHeight, CapsuleRadius, FQuat::Identity, FColor::Cyan, false, 2.0f, 0, 1.f);
    }
	
    return true;
}