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

            if (!TargetASC->HasMatchingGameplayTag(PrologueGameplayTags::Shared_State_NoHitEffect))
            {
                bHitNormalTarget = true;
            }

            bHitDetected = true;
            
            FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect);
            FGameplayEffectSpecHandle HitReactEffectSpecHandle = MakeOutgoingGameplayEffectSpec(ToughnessDamageEffect);
            
            if (HitReactEffectSpecHandle.IsValid())
            {
                if (!TargetASC->HasMatchingGameplayTag(PrologueGameplayTags::Shared_State_NoHitEffect))
                {
                    // 강인도 감소 적용
                    TargetASC->ApplyGameplayEffectSpecToSelf(*HitReactEffectSpecHandle.Data.Get());
                }
            }
            
            if (EffectSpecHandle.IsValid())
            {
                // 대미지 적용
                TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

                if (!TargetASC->HasMatchingGameplayTag(PrologueGameplayTags::Shared_State_NoHitEffect))
                {
                    // VFX용 GameplayCue
                    FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
                    CueContextHandle.AddHitResult(HitResult);
                    FGameplayCueParameters CueParam;
                    CueParam.EffectContext = CueContextHandle;

                    // 피격 이펙트 출력
                    if (Cast<AComma>(GetAvatarActorFromActorInfo()))
                    {
                        FGameplayEventData FxEventData;
                        TargetASC->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_EnemyHit, CueParam);
                        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitResult.GetActor(), PrologueGameplayTags::Shared_Event_HitFx, FxEventData);
                    }
                    else
                    {
                        TargetASC->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_PlayerHit, CueParam);
                    }
                }
            }
        }
        else if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, i))
        {
            TArray<AActor*> Actors = UAbilitySystemBlueprintLibrary::GetActorsFromTargetData(TargetDataHandle, i);

            for (AActor* TargetActor : Actors)
            {
                UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
                if (!TargetASC)
                {
                    continue;
                }
                
                if (!TargetASC->HasMatchingGameplayTag(PrologueGameplayTags::Shared_State_NoHitEffect))
                {
                    bHitNormalTarget = true;
                }
                
                FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect);
                FGameplayEffectSpecHandle HitReactEffectSpecHandle = MakeOutgoingGameplayEffectSpec(ToughnessDamageEffect);

                if (HitReactEffectSpecHandle.IsValid())
                {
                    if (!TargetASC->HasMatchingGameplayTag(PrologueGameplayTags::Shared_State_NoHitEffect))
                    {
                        // 강인도 감소
                        ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, HitReactEffectSpecHandle, TargetDataHandle);
                    }
                }

                if (EffectSpecHandle.IsValid())
                {
                    TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

                    if (!TargetASC->HasMatchingGameplayTag(PrologueGameplayTags::Shared_State_NoHitEffect))
                    {
                        FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
                        CueContextHandle.AddActors({TargetActor}, false);
                        FGameplayCueParameters CueParam;
                        CueParam.EffectContext = CueContextHandle;

                        // 피격 이펙트 출력
                        FGameplayEventData FxEventData;
                        TargetASC->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_EnemySmashHit, CueParam);
                        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, PrologueGameplayTags::Shared_Event_HitFx, FxEventData);
                    }
                }
            }
        }
    }

    if (bHitNormalTarget)
    {
        if (TargetDataHandle.Num() > 0 && UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0))
        {
            if (bHitDetected)
            {
                // 일반 공격 피격 사운드
                GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_DamagingSound);
            }
            
            // 일반 공격 카메라 쉐이킹
            GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_Damaging);
        }
        else if(TargetDataHandle.Num() > 0 && UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, 0))
        {
            // 스매쉬 공격 전용 카메라 쉐이킹, 피격 사운드
            GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_SmashAttackDamaging);
            GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_SmashDamagingSound);
        }
    }
    
    bool bReplicatedEndAbility = true;
    bool bWasCancelled = false;
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}