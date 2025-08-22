// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_BrokenCheck.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AT/AT_WaitForTrace.h"
#include "Prologue/Prologue.h"

UGA_BrokenCheck::UGA_BrokenCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_BrokenCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	LOG_SCREEN("%s", *LOG_CALLINFO);

	UAT_WaitForTrace* AttackTraceTask = UAT_WaitForTrace::CreateTask(this, TargetActorClass);
	AttackTraceTask->OnComplete.AddDynamic(this, &UGA_BrokenCheck::OnTraceResultCallback);
	AttackTraceTask->ReadyForActivation();
}

void UGA_BrokenCheck::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
    for (int32 i = 0; i < TargetDataHandle.Num(); i++)
    {
        if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, i))
        {
            FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, i);
            
            UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());
            if (!TargetASC)
            {
                continue;
            }
        	
            FGameplayEffectSpecHandle BrokenEffectSpecHandle = MakeOutgoingGameplayEffectSpec(BrokenDamageEffect);

            if (BrokenEffectSpecHandle.IsValid())
            {
                TargetASC->ApplyGameplayEffectSpecToSelf(*BrokenEffectSpecHandle.Data.Get());
            }
        }
    }
    
    bool bReplicatedEndAbility = true;
    bool bWasCancelled = false;
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
