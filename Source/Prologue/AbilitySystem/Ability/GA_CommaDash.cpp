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

	FVector StartPos = GetAvatarActorFromActorInfo()->GetActorLocation();

	FVector InputDirection = MovementComponent->GetLastInputVector();
	InputDirection.Normalize();

	if (InputDirection.IsNearlyZero())
	{
		InputDirection = Comma->GetActorForwardVector();
	}
	
	FVector TentativeEndPos = StartPos + InputDirection * MoveLength;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Comma);
	FHitResult HIt;

	bool bResult = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		StartPos,
		TentativeEndPos,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		IgnoreActors,
		EDrawDebugTrace::ForDuration,
		HIt,
		true
	);

	FVector CheckedEndPos = bResult ? HIt.ImpactPoint : TentativeEndPos;

	FHitResult GroundHit;
	FVector GroundTraceStart = CheckedEndPos + FVector(0.f, 0.f, GroundTraceUpOffset);
	FVector GroundTraceEnd = CheckedEndPos - FVector(0.f, 0.f, GroundTraceDistance);

	bool bHitGround = UKismetSystemLibrary::LineTraceSingle(
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
		5.0f
	);

	BasePos = StartPos;
	if (bHitGround)
	{
		float CapsuleHalfHeight = Comma->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		TargetPos = CheckedEndPos;
		TargetPos.Z = GroundHit.ImpactPoint.Z + CapsuleHalfHeight + TargetZOffset;
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	if (FVector::DistSquared(BasePos, TargetPos) < MinDashDistance * MinDashDistance)
	{
		LOG_SCREEN("Dash distance too short.");
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
		
	TickCurve->ReadyForActivation();
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
