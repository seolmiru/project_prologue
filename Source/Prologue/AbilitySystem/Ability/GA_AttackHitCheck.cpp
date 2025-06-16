// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_AttackHitCheck.h"

#include "Prologue/Prologue.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GA_CommaAttackSword.h"
#include "AT/AT_WaitForTrace.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/Character/Player/Comma.h"
#include "TA/TA_Trace.h"

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

	CurrentComboIndex = 0;
	if (AComma* Comma = Cast<AComma>(GetAvatarActorFromActorInfo()))
	{
		CurrentComboIndex = Comma->CurrentSwordCombo - 1;
		CurrentComboIndex = FMath::Max(0, CurrentComboIndex);
		LOG_SCREEN_R("Current Combo Index : %d", CurrentComboIndex);
	}

	TSubclassOf<ATA_Trace> SelectedTargetActorClass = TargetActorClass;
	if (ComboTargetActorClasses.IsValidIndex(CurrentComboIndex) && ComboTargetActorClasses[CurrentComboIndex])
	{
		SelectedTargetActorClass = ComboTargetActorClasses[CurrentComboIndex];
		LOG_SCREEN_R("Using Combo TargetActor for index : %d", CurrentComboIndex);
	}

	if (!SelectedTargetActorClass)
	{
		LOG_SCREEN_R("No valid TargetActorClass");
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAT_WaitForTrace* AttackTraceTask = UAT_WaitForTrace::CreateTask(this, SelectedTargetActorClass);
	AttackTraceTask->OnComplete.AddDynamic(this, &UGA_AttackHitCheck::OnTraceResultCallback);
	AttackTraceTask->ReadyForActivation();
}

void UGA_AttackHitCheck::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	TSubclassOf<UGameplayEffect> SelectedDamageEffect = AttackDamageEffect;
	if (ComboAttackDamageEffects.IsValidIndex(CurrentComboIndex) && ComboAttackDamageEffects[CurrentComboIndex])
	{
		SelectedDamageEffect = ComboAttackDamageEffects[CurrentComboIndex];
		LOG_SCREEN_R("Using Combo DamageEffect for index : %d", CurrentComboIndex);
	}
	
	if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0))
	{
		FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(SelectedDamageEffect);
		FGameplayEffectSpecHandle HitReactEffectSpecHandle = MakeOutgoingGameplayEffectSpec(ToughnessDamageEffect);

		// 강인도 감소 적용
		if (HitReactEffectSpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, HitReactEffectSpecHandle, TargetDataHandle);
		}
		
		if (EffectSpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);

			// 공격이 적중했을 때, 카메라 쉐이킹, VFX 연출을 위해 Effect 부여
			GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_Damaging);

			FVector AttackerLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
			FVector HitLocation = HitResult.Location;
			FVector HitDirection = (HitLocation - AttackerLocation).GetSafeNormal();

			AActor* HitActor = HitResult.GetActor();
			FVector TargetCenter = HitActor->GetActorLocation();
			
			FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
			CueContextHandle.AddHitResult(HitResult);
			FGameplayCueParameters CueParam;
			CueParam.EffectContext = CueContextHandle;

			FVector EffectLocation = TargetCenter;
			EffectLocation.Z += 50.f;
			CueParam.Location = EffectLocation;

			CueParam.Normal = HitDirection;
			CueParam.Instigator = GetAvatarActorFromActorInfo();
			CueParam.EffectCauser = HitActor;
			
			if (Cast<AComma>(GetAvatarActorFromActorInfo()))
			{
				UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor())->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_EnemyHit, CueParam);
			}
			else
			{
				UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor())->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_PlayerHit, CueParam);
			}
		}
	}
	else if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, 0))
	{
		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();

		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect);
		FGameplayEffectSpecHandle HitReactEffectSpecHandle = MakeOutgoingGameplayEffectSpec(ToughnessDamageEffect);

		if (HitReactEffectSpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, HitReactEffectSpecHandle, TargetDataHandle);
		}
		
		if (EffectSpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);

			// 교체 공격 전용 카메라 쉐이킹 연출을 위한 Effect 부여
			GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_SwitchAttackDamaging);

			FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
			CueContextHandle.AddActors(TargetDataHandle.Data[0].Get()->GetActors(), false);
			FGameplayCueParameters CueParam;
			CueParam.EffectContext = CueContextHandle;

			SourceASC->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_EnemyHit, CueParam);
		}

		// 오버클락 게이지 증가 이펙트 부여
		if (IncreaseGaugeEffect)
		{
			FGameplayEffectSpecHandle GaugeEffectSpecHandle = MakeOutgoingGameplayEffectSpec(IncreaseGaugeEffect);
			if (GaugeEffectSpecHandle.IsValid())
			{
				UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
				ASC->ApplyGameplayEffectSpecToSelf(*GaugeEffectSpecHandle.Data.Get());
			}
		}
	}

	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
