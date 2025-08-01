// Fill out your copyright notice in the Description page of Project Settings.


#include "Prologue/AbilitySystem/Ability/TA/TA_Trace.h"

#include "Abilities/GameplayAbility.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Prologue/Prologue.h"

ATA_Trace::ATA_Trace()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATA_Trace::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
}

void ATA_Trace::ConfirmTargetingAndContinue()
{
	if (SourceActor)
	{
		FGameplayAbilityTargetDataHandle DataHandle = MakeTargetData();
		TargetDataReadyDelegate.Broadcast(DataHandle);
	}
}

FGameplayAbilityTargetDataHandle ATA_Trace::MakeTargetData() const
{
	ACharacter* Character = CastChecked<ACharacter>(SourceActor);
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Character);

	FGameplayAbilityTargetDataHandle DataHandle;
	TSet<AActor*> HitActors;

	if (bUseFanShapeTrace)
	{
		const FVector Forward = Character->GetActorForwardVector();
		const FVector StartBase = Character->GetActorLocation() + Forward * Character->GetCapsuleComponent()->GetScaledCapsuleRadius();

		const float HalfFanAngle = Angle * 0.5f;
		const float AngleStep = (NumTraces > 1) ? Angle / (NumTraces - 1) : 0.0f;

		for (int32 i = 0; i < NumTraces; ++i)
		{
			float CurrentAngle = -HalfFanAngle + (AngleStep * i);
			
			FRotator Rotation(0.0f, CurrentAngle, 0.0f);
			FVector TraceDirection = Rotation.RotateVector(Forward);
			
			FVector Start = StartBase;
			FVector End = Start + TraceDirection * TraceLength;

			TArray<FHitResult> HitResults;
			bool bTraceResult = UKismetSystemLibrary::SphereTraceMulti(
				GetWorld(),
				Start,
				End,
				TraceRadius,
				UEngineTypes::ConvertToTraceType(TraceChannel),
				false,
				IgnoreActors,
				bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
				HitResults,
				false,
				FLinearColor::Red,
				FLinearColor::Green,
				1.0f
			);

			if (bTraceResult)
			{
				for (const FHitResult& HitResult : HitResults)
				{
					if (HitResult.GetActor() && !HitActors.Contains(HitResult.GetActor()))
					{
						HitActors.Add(HitResult.GetActor());
						FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
						DataHandle.Add(TargetData);
					}
				}
			}
		}
	}
	else
	{
		TArray<FHitResult> HitResults;
		const FVector Forward = Character->GetActorForwardVector();
		const FVector Start = Character->GetActorLocation() + Forward * Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
		const FVector End = Start + Forward * TraceLength;

		bool bTraceResult = UKismetSystemLibrary::SphereTraceMulti(
			GetWorld(),
			Start,
			End,
			TraceRadius,
			UEngineTypes::ConvertToTraceType(TraceChannel),
			false,
			IgnoreActors,
			bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
			HitResults,
			false
		);

		if (bTraceResult)
		{
			for (const FHitResult& HitResult : HitResults)
			{
				if (HitResult.GetActor() && !HitActors.Contains(HitResult.GetActor()))
				{
					HitActors.Add(HitResult.GetActor());
					FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
					DataHandle.Add(TargetData);
				}
			}
		}
	}
	
	return DataHandle;
}
