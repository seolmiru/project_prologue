// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_MangoAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AT/AT_TickSocketTrace.h"

void UGA_MangoAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	GetWorld()->GetTimerManager().SetTimer(TraceStartTimerHandle, this, &UGA_MangoAttack::StartSocketTrace, TraceStartDelay, false);
}

void UGA_MangoAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	GetWorld()->GetTimerManager().ClearTimer(TraceStartTimerHandle);

	if (SocketTraceTask)
	{
		SocketTraceTask->EndTask();
		SocketTraceTask = nullptr;
	}
}

void UGA_MangoAttack::OnTraceResult(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0))
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect);
		if (EffectSpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
		}
	}
}

void UGA_MangoAttack::StartSocketTrace()
{
	SocketTraceTask = UAT_TickSocketTrace::TickSocketTrace(this);

	if (SocketTraceTask)
	{
		SocketTraceTask->TotalDuration = AttackDuration;
		SocketTraceTask->StartSocketName = StartSocketName;
		SocketTraceTask->EndSocketName = EndSocketName;
		SocketTraceTask->TraceRadius = TraceRadius;
		SocketTraceTask->bShowDebug = bShowDebug;

		SocketTraceTask->OnTraceResultCallback.AddDynamic(this, &UGA_MangoAttack::OnTraceResult);

		SocketTraceTask->ReadyForActivation();
	}
}
