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

	FHitResult HitResult;
	const FVector Forward = Character->GetActorForwardVector();
	const FVector Start = Character->GetActorLocation() + Forward * Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + Forward * 150.0f;

	bool bTraceResult = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		Start,
		End,
		50.f,
		UEngineTypes::ConvertToTraceType(TraceChannel),
		false,
		IgnoreActors,
		bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResult,
		false);

	FGameplayAbilityTargetDataHandle DataHandle;
	if (bTraceResult)
	{
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		DataHandle.Add(TargetData);
	}
	
	return DataHandle;
}
