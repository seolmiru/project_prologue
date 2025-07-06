// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_MangoLaserAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AT/AT_TickBoxTrace.h"

bool UGA_MangoLaserAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;	
	}

	return !ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Shared.Cooldown.Attack.Laser")));
}

void UGA_MangoLaserAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, true))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	BoxTraceTask = UAT_TickBoxTrace::TickBoxTrace(this);
	if (BoxTraceTask)
	{
		BoxTraceTask->TotalDuration = LaserDuration;
		BoxTraceTask->BoxHalfSize = LaserBoxHalfSize;
		BoxTraceTask->TraceLength = LaserLength;
		BoxTraceTask->DamageInterval = DamageTickInterval;
		BoxTraceTask->bShowDebug = bShowDebugTrace;
		
		BoxTraceTask->OnTraceResultCallback.AddDynamic(this, &UGA_MangoLaserAttack::OnTraceResultCallback);
		BoxTraceTask->ReadyForActivation();

		GetWorld()->GetTimerManager().SetTimer(LaserTimerHandle, this, &UGA_MangoLaserAttack::OnTraceFinished, LaserDuration, false);
	}
}

void UGA_MangoLaserAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	GetWorld()->GetTimerManager().ClearTimer(LaserTimerHandle);
	
	if (BoxTraceTask)
	{
		BoxTraceTask->EndTask();
		BoxTraceTask = nullptr;
	}
}

void UGA_MangoLaserAttack::OnTraceFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_MangoLaserAttack::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0))
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
		if (EffectSpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
		}
	}
}
