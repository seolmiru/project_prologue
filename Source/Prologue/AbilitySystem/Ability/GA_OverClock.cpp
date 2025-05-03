// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_OverClock.h"

#include "Kismet/GameplayStatics.h"
#include "Prologue/AbilitySystem/PrologueAttributeSet.h"
#include "Prologue/Character/Player/Comma.h"

UGA_OverClock::UGA_OverClock()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_OverClock::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	const UPrologueAttributeSet* AttributeSet = ASC->GetSet<UPrologueAttributeSet>();

	if (AttributeSet->GetCurrentGauge() < AttributeSet->GetMaxGauge())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	const_cast<UPrologueAttributeSet*>(AttributeSet)->SetCurrentGauge(0.0f);

	const float TimerDelay = OverClockDuration * GlobalTimeScale;
	GetWorld()->GetTimerManager().SetTimer(
		OverClockTimerHandle,
		this,
		&UGA_OverClock::OnOverClockFinished,
		TimerDelay,
		false
	);
	
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), GlobalTimeScale);

	if (AComma* Comma = CastChecked<AComma>(ActorInfo->AvatarActor.Get()))
	{
		Comma->CustomTimeDilation = 1.0f / FMath::Max(GlobalTimeScale, KINDA_SMALL_NUMBER);
	}
}

void UGA_OverClock::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	GetWorld()->GetTimerManager().ClearTimer(OverClockTimerHandle);
}

void UGA_OverClock::OnOverClockFinished()
{
	LOG_SCREEN("End OverClock. Restoring time.");
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);

	if (AComma* Comma = CastChecked<AComma>(GetAvatarActorFromActorInfo()))
	{
		Comma->CustomTimeDilation = 1.0f;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
