// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CommaAttackSword.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/Character/Player/Comma.h"
#include "Prologue/DataAsset/ComboBowData.h"
#include "Prologue/DataAsset/ComboSwordData.h"

UGA_CommaAttackSword::UGA_CommaAttackSword()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_CommaAttackSword::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return !ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Comma.State.SwitchAttack.Sword")));
}

void UGA_CommaAttackSword::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AComma* Comma = CastChecked<AComma>(ActorInfo->AvatarActor.Get());
	
	CurrentComboData = Comma->GetComboSwordData();

	Comma->RotateToMouseSmooth();
	Comma->GetSwordWeaponMesh()->SetVisibility(true);
	Comma->GetBowWeaponMesh()->SetVisibility(false);
	
	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayAttack"), Comma->GetSwordComboMontage(), 1.0f, GetNextSection());
	PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_CommaAttackSword::OnComplete);
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_CommaAttackSword::OnInterrupted);
	PlayAttackTask->OnBlendOut.AddDynamic(this, &UGA_CommaAttackSword::OnBlendOut);
	PlayAttackTask->ReadyForActivation();
	
	GetWorld()->GetTimerManager().ClearTimer(CurrentComboTimerHandle);
	
	StartComboTimer();
}

void UGA_CommaAttackSword::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if (!ComboTimerHandle.IsValid())
	{
		ProcessNextCombo();
	}
	else
	{
		HasNextComboInput = true;
	}
}

void UGA_CommaAttackSword::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	if (CurrentComboData && CurrentCombo == CurrentComboData->MaxComboCount)
	{
		ResetComboCount();
	}
	
	CurrentComboData = nullptr;
	HasNextComboInput = false;
}

void UGA_CommaAttackSword::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	GetWorld()->GetTimerManager().SetTimer(CurrentComboTimerHandle, this, &UGA_CommaAttackSword::ResetComboCount, 1.2f, false);

	if (AComma* Comma = CastChecked<AComma>(CurrentActorInfo->AvatarActor.Get()))
	{
		Comma->OnAttackEnded();
	}
	
	// 마지막 콤보 실행 직후 교체 공격 Effect 부여
	if (CurrentComboData && CurrentCombo == CurrentComboData->MaxComboCount)
	{
		ResetComboCount();
		
		FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle EffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(SwitchAttackEffectClass, 0.0f, EffectContextHandle);
		GetAbilitySystemComponentFromActorInfo()->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
	}
	
	CurrentComboData = nullptr;
	HasNextComboInput = false;
}

void UGA_CommaAttackSword::OnComplete()
{
	if (!HasNextComboInput && !GetWorld()->GetTimerManager().IsTimerActive(ComboTimerHandle))
	{
		bool bReplicatedEndAbility = true;
		bool bWasCancelled = false;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
	}
}

void UGA_CommaAttackSword::OnInterrupted()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UGA_CommaAttackSword::OnBlendOut()
{
	if (!HasNextComboInput && !GetWorld()->GetTimerManager().IsTimerActive(ComboTimerHandle))
	{
		bool bReplicatedEndAbility = true;
		bool bWasCancelled = false;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
	}
}

FName UGA_CommaAttackSword::GetNextSection()
{
	LOG_SCREEN_R("GetNextSection - Before : %d", CurrentCombo);
	
	CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, CurrentComboData->MaxComboCount);
	FName NextSection = *FString::Printf(TEXT("%s%d"), *CurrentComboData->MontageSectionNamePrefix, CurrentCombo);
	return NextSection;
}

void UGA_CommaAttackSword::StartComboTimer()
{
	int32 ComboIndex = CurrentCombo - 1;
	ensure(CurrentComboData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	// 프레임 수를 시간으로 변환
	const float ComboEffectiveTime = CurrentComboData->EffectiveFrameCount[ComboIndex] / CurrentComboData->FrameRate;

	// 유효 시간이 있다면 타이머 설정, 없으면 즉시 콤보 입력 허용
	if (ComboEffectiveTime > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &UGA_CommaAttackSword::CheckComboInput, ComboEffectiveTime, false);
	}
	else
	{
		EnableComboInput();
	}
}

void UGA_CommaAttackSword::CheckComboInput()
{
	ComboTimerHandle.Invalidate();
	if (HasNextComboInput)
	{
		// 최대 콤보 수 도달 시에는 콤보 진행 제어
		if (CurrentCombo >= CurrentComboData->MaxComboCount)
		{
			HasNextComboInput = false;
			return;
		}

		AComma* Comma = CastChecked<AComma>(CurrentActorInfo->AvatarActor.Get());
		Comma->RotateToMouseSmooth();
		
		MontageJumpToSection(GetNextSection());
		StartComboTimer();
		HasNextComboInput = false;
	}
}

void UGA_CommaAttackSword::ResetComboCount()
{
	CurrentCombo = 0;
	LOG_SCREEN_R("AttackSword : Reset Combo Count");
}

void UGA_CommaAttackSword::EnableComboInput()
{
	ComboTimerHandle.Invalidate();

	if (HasNextComboInput)
	{
		ProcessNextCombo();
	}
}

void UGA_CommaAttackSword::ProcessNextCombo()
{
	LOG_SCREEN_R("ProcessNextCombo - Current : %d", CurrentCombo);
	
	if (CurrentCombo >= CurrentComboData->MaxComboCount)
	{
		return;
	}

	AComma* Comma = CastChecked<AComma>(CurrentActorInfo->AvatarActor.Get());
	Comma->RotateToMouseSmooth();
	
	MontageJumpToSection(GetNextSection());
	StartComboTimer();
	HasNextComboInput = false;
}
