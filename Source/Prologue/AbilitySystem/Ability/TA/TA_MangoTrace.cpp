// Fill out your copyright notice in the Description page of Project Settings.


#include "TA_MangoTrace.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

FGameplayAbilityTargetDataHandle ATA_MangoTrace::MakeTargetData() const
{
	ACharacter* Character = CastChecked<ACharacter>(SourceActor);
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Character);

	FGameplayAbilityTargetDataHandle DataHandle;
	FHitResult HitResult;
	
	const FVector BoxCenter = Character->GetActorLocation() + Character->GetActorTransform().TransformVector(BoxOffset);

	bool bTraceResult = UKismetSystemLibrary::BoxTraceSingle(
		GetWorld(),
		BoxCenter,
		BoxCenter,
		BoxHalfSize,
		Character->GetActorRotation(),
		UEngineTypes::ConvertToTraceType(TraceChannel),
		false,
		IgnoreActors,
		bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		1.f
	);

	if (bTraceResult)
	{
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		DataHandle.Add(TargetData);
	}
	
	return DataHandle;
}

