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

	FHitResult HitResult;
	const FVector Forward = Character->GetActorForwardVector();
	const FVector Start = Character->GetActorLocation() + Forward * Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + Forward * 270.0f;

	bool bResult = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		Start,
		End,
		120.0f,
		UEngineTypes::ConvertToTraceType(TraceChannel),
		false,
		IgnoreActors,
		bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResult,
		false);
	
	FGameplayAbilityTargetDataHandle DataHandle;
	if (bResult)
	{
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		DataHandle.Add(TargetData);
		LOG_SCREEN("Trace Check");
	}

	return DataHandle;
}
