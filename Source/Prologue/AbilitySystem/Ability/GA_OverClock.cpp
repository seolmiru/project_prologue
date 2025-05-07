// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_OverClock.h"

#include "Kismet/GameplayStatics.h"
#include "Prologue/AbilitySystem/PrologueAttributeSet.h"
#include "Prologue/Character/Enemy/PrologueEnemyCharacter.h"

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

	ApplySlowToEnemies();
	
	GetWorld()->GetTimerManager().SetTimer(
		OverClockTimerHandle,
		this,
		&UGA_OverClock::OnOverClockFinished,
		OverClockDuration,
		false
	);
}

void UGA_OverClock::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	GetWorld()->GetTimerManager().ClearTimer(OverClockTimerHandle);

	RestoreEnemyTime();
}

void UGA_OverClock::OnOverClockFinished()
{
	LOG_SCREEN_R("End OverClock. Restoring time.");

	RestoreEnemyTime();
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_OverClock::ApplySlowToEnemies()
{
	AffectedEnemies.Empty();

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		APrologueEnemyCharacter::StaticClass(),
		Found
	);

	for (AActor* Actor : Found)
	{
		if (auto* Enemy = Cast<APrologueEnemyCharacter>(Actor))
		{
			Enemy->CustomTimeDilation = TimeScale;
			AffectedEnemies.Add(Enemy);
		}
	}
}

void UGA_OverClock::RestoreEnemyTime()
{
	for (auto* Enemy : AffectedEnemies)
	{
		if (IsValid(Enemy))
		{
			Enemy->CustomTimeDilation = 1.0f;
		}
	}
	AffectedEnemies.Empty();
}
