// Fill out your copyright notice in the Description page of Project Settings.


#include "TA_EnemyTrace.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Prologue/Prologue.h"
#include "Prologue/AbilitySystem/PrologueAttributeSet.h"

FGameplayAbilityTargetDataHandle ATA_EnemyTrace::MakeTargetData() const
{
	ACharacter* Character = CastChecked<ACharacter>(SourceActor);
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Character);

	FGameplayAbilityTargetDataHandle DataHandle;

	if (bUseFanShapeTrace)
	{
		TSet<AActor*> HitActors;
		
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

			FHitResult HitResult;
			bool bTraceResult = UKismetSystemLibrary::SphereTraceSingle(
				GetWorld(),
				Start,
				End,
				TraceRadius,
				UEngineTypes::ConvertToTraceType(TraceChannel),
				false,
				IgnoreActors,
				bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
				HitResult,
				false,
				FLinearColor::Red,
				FLinearColor::Green,
				1.0f
			);

			if (bTraceResult && HitResult.GetActor())
			{
				AActor* HitActor = HitResult.GetActor();

				if (!HitActors.Contains(HitActor))
				{
					HitActors.Add(HitActor);
					FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
					DataHandle.Add(TargetData);
				}
			}
		}
	}
	else
	{
		FHitResult HitResult;
		const FVector Forward = Character->GetActorForwardVector();
		const FVector Start = Character->GetActorLocation() + Forward * Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
		const FVector End = Start + Forward * TraceLength;

		bool bTraceResult = UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			Start,
			End,
			TraceRadius,
			UEngineTypes::ConvertToTraceType(TraceChannel),
			false,
			IgnoreActors,
			bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
			HitResult,
			false
		);

		if (bTraceResult)
		{
			FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
			DataHandle.Add(TargetData);
		}
	}
	
	return DataHandle;
}
