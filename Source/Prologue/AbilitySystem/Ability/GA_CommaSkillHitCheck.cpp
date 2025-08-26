// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CommaSkillHitCheck.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AT/AT_WaitForTrace.h"
#include "Prologue/Prologue.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/Character/Player/Comma.h"

UGA_CommaSkillHitCheck::UGA_CommaSkillHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_CommaSkillHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	LOG_SCREEN("%s", *LOG_CALLINFO);

	UAT_WaitForTrace* AttackTraceTask = UAT_WaitForTrace::CreateTask(this, TargetActorClass);
	AttackTraceTask->OnComplete.AddDynamic(this, &UGA_CommaSkillHitCheck::OnTraceResultCallback);
	AttackTraceTask->ReadyForActivation();
}

void UGA_CommaSkillHitCheck::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	bool bHitDetected = false;

    bool bHitNormalTarget = false;
    
    for (int32 i = 0; i < TargetDataHandle.Num(); ++i)
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
            FGameplayEffectSpecHandle BrokenEffectSpecHandle = MakeOutgoingGameplayEffectSpec(BrokenDamageEffect);

            if (EffectSpecHandle.IsValid())
            {
                // 대미지 적용
                TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

                // 경직 적용
                FGameplayEventData StunEventData;
                UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitResult.GetActor(), PrologueGameplayTags::Enemy_Event_Stun, StunEventData);

                if (!TargetASC->HasMatchingGameplayTag(PrologueGameplayTags::Shared_State_NoHitEffect))
                {
                    // VFX용 GameplayCue
                    FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
                    CueContextHandle.AddHitResult(HitResult);
                    FGameplayCueParameters CueParam;
                    CueParam.EffectContext = CueContextHandle;

                    // 피격 이펙트 출력
                    FGameplayEventData FxEventData;
                    TargetASC->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_EnemyHit, CueParam);
                    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitResult.GetActor(), PrologueGameplayTags::Shared_Event_HitFx, FxEventData);
                }
            }

            if (BrokenEffectSpecHandle.IsValid())
            {
                TargetASC->ApplyGameplayEffectSpecToSelf(*BrokenEffectSpecHandle.Data.Get());
            }
        }
    }

    if (bHitNormalTarget)
    {
        if (TargetDataHandle.Num() > 0 && UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0))
        {
            if (bHitDetected)
            {
                // 피격 사운드
                GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_DamagingSound);
            }
            
            // 카메라 쉐이킹
            GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_Damaging);

            // HitStopEventData 전송
            FGameplayEventData HitStopEventData;
            UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActorFromActorInfo(), PrologueGameplayTags::Comma_Event_HitStop, HitStopEventData);
        }
    }
    
    bool bReplicatedEndAbility = true;
    bool bWasCancelled = false;
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
