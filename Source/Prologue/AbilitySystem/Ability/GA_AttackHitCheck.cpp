// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_AttackHitCheck.h"

#include "Prologue/Prologue.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AT/AT_WaitForTrace.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/Character/Player/Comma.h"

UGA_AttackHitCheck::UGA_AttackHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_AttackHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	LOG_SCREEN("%s", *LOG_CALLINFO);

	UAT_WaitForTrace* AttackTraceTask = UAT_WaitForTrace::CreateTask(this, TargetActorClass);
	AttackTraceTask->OnComplete.AddDynamic(this, &UGA_AttackHitCheck::OnTraceResultCallback);
	AttackTraceTask->ReadyForActivation();
}

void UGA_AttackHitCheck::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
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
            
            FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect);
            FGameplayEffectSpecHandle HitReactEffectSpecHandle = MakeOutgoingGameplayEffectSpec(ToughnessDamageEffect);
            
            if (HitReactEffectSpecHandle.IsValid())
            {
                TargetASC->ApplyGameplayEffectSpecToSelf(*HitReactEffectSpecHandle.Data.Get());
            }
            
            if (EffectSpecHandle.IsValid())
            {
                TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
                
                // VFX용 GameplayCue
                FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
                CueContextHandle.AddHitResult(HitResult);
                FGameplayCueParameters CueParam;
                CueParam.EffectContext = CueContextHandle;
                
                if (Cast<AComma>(GetAvatarActorFromActorInfo()))
                {
                    TargetASC->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_EnemyHit, CueParam);
                }
                else
                {
                    TargetASC->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_PlayerHit, CueParam);
                }
            }
        }
        else if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, i))
        {
            TArray<AActor*> Actors = UAbilitySystemBlueprintLibrary::GetActorsFromTargetData(TargetDataHandle, i);

            for (AActor* TargetActor : Actors)
            {
                UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

                FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect);
                FGameplayEffectSpecHandle HitReactEffectSpecHandle = MakeOutgoingGameplayEffectSpec(ToughnessDamageEffect);

                if (HitReactEffectSpecHandle.IsValid())
                {
                    ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, HitReactEffectSpecHandle, TargetDataHandle);
                }

                if (EffectSpecHandle.IsValid())
                {
                    TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
                    
                    FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
                    CueContextHandle.AddActors({TargetActor}, false);
                    FGameplayCueParameters CueParam;
                    CueParam.EffectContext = CueContextHandle;

                    TargetASC->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_EnemySmashHit, CueParam);
                }
            }

            GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_SmashAttackDamaging);
            
            // 오버클락 게이지 증가
            if (IncreaseGaugeEffect)
            {
                FGameplayEffectSpecHandle GaugeEffectSpecHandle = MakeOutgoingGameplayEffectSpec(IncreaseGaugeEffect);
                if (GaugeEffectSpecHandle.IsValid())
                {
                    GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*GaugeEffectSpecHandle.Data.Get());
                }
            }
        }
    }
    
    // 카메라 쉐이킹, 피격 사운드 적용
    if (TargetDataHandle.Num() > 0)
    {
        GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_Damaging);

        if (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(PrologueGameplayTags::Comma_Event_Sword))
        {
            GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_DamagingSound);
        }
        else
        {
            GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_SmashDamagingSound);
        }
    }
    
    bool bReplicatedEndAbility = true;
    bool bWasCancelled = false;
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}