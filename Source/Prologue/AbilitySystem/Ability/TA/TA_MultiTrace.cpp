// Fill out your copyright notice in the Description page of Project Settings.


#include "TA_MultiTrace.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "Prologue/Prologue.h"
#include "Prologue/AbilitySystem/PrologueAttributeSet.h"

FGameplayAbilityTargetDataHandle ATA_MultiTrace::MakeTargetData() const
{
	ACharacter* Character = CastChecked<ACharacter>(SourceActor);

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);
	if (!ASC)
	{
		LOG_SCREEN("ASC not found");
		return FGameplayAbilityTargetDataHandle();
	}

	const UPrologueAttributeSet* AttributeSet = ASC->GetSet<UPrologueAttributeSet>();
	if (!AttributeSet)
	{
		LOG_SCREEN("Attribute not found");
		return FGameplayAbilityTargetDataHandle();
	}

	TArray<FOverlapResult> Overlaps;
	const float SwitchAttackRadius = AttributeSet->GetSwordSwitchAttackRange();

	FVector Origin = Character->GetActorLocation();
	FCollisionQueryParams Params(SCENE_QUERY_STAT(ATA_MultiTrace), false, Character);
	GetWorld()->OverlapMultiByChannel(
		Overlaps,
		Origin,
		FQuat::Identity,
		TraceChannel,
		FCollisionShape::MakeSphere(SwitchAttackRadius),
		Params
	);

	TArray<TWeakObjectPtr<AActor>> HitActors;
	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* HitActor = Overlap.OverlapObjectHandle.FetchActor<AActor>();
		if (HitActor && !HitActors.Contains(HitActor))
		{
			HitActors.Add(HitActor);
		}
	}

	FGameplayAbilityTargetData_ActorArray* ActorsData = new FGameplayAbilityTargetData_ActorArray();
	ActorsData->SetActors(HitActors);

#if ENABLE_DRAW_DEBUG

	if (bShowDebug)
	{
		FColor DrawColor = HitActors.Num() > 0 ? FColor::Green : FColor::Red;
		DrawDebugSphere(
			GetWorld(),
			Origin,
			300.f,
			16,
			DrawColor,
			false,
			5.0f
		);
	}
	
#endif

	return FGameplayAbilityTargetDataHandle(ActorsData);
}
