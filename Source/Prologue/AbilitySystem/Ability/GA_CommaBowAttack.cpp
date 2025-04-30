// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CommaBowAttack.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/Character/Comma.h"
#include "Prologue/DataAsset/ComboBowData.h"

UGA_CommaBowAttack::UGA_CommaBowAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_CommaBowAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AComma* Comma = CastChecked<AComma>(ActorInfo->AvatarActor.Get());
	
	CurrentComboData = Comma->GetComboBowData();
		
	Comma->RotateToMouse();
	Comma->GetSwordWeaponMesh()->SetVisibility(false);
	Comma->GetBowWeaponMesh()->SetVisibility(true);

	if (Comma->GetAbilitySystemComponent()->HasMatchingGameplayTag(PrologueGameplayTags::Comma_State_SwitchAttack_Bow))
	{
		UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayAttack"), Comma->GetBowSwitchAttackMontage(), 1.0f, GetNextSection());
		PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_CommaBowAttack::OnComplete);
		PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_CommaBowAttack::OnInterrupted);
		PlayAttackTask->ReadyForActivation();
	}
	else
	{
		UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayAttack"), Comma->GetBowComboMontage(), 1.0f, GetNextSection());
		PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_CommaBowAttack::OnComplete);
		PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_CommaBowAttack::OnInterrupted);
		PlayAttackTask->ReadyForActivation();
	}
	
	StartComboTimer();
}

void UGA_CommaBowAttack::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if (!ComboTimerHandle.IsValid())
	{
		HasNextComboInput = false;
	}
	else
	{
		HasNextComboInput = true;
	}
}

void UGA_CommaBowAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	CurrentComboData = nullptr;
	CurrentCombo = 0;
	HasNextComboInput = false;
}

void UGA_CommaBowAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	CurrentComboData = nullptr;
	CurrentCombo = 0;
	HasNextComboInput = false;
}

void UGA_CommaBowAttack::OnComplete()
{
	if (CurrentComboData && CurrentCombo == CurrentComboData->MaxComboCount)
	{
		FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle EffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(SwitchAttackEffectClass, 0.0f, EffectContextHandle);
		GetAbilitySystemComponentFromActorInfo()->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
	}
	
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UGA_CommaBowAttack::OnInterrupted()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

FName UGA_CommaBowAttack::GetNextSection()
{
	CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, CurrentComboData->MaxComboCount);
	FName NextSection = *FString::Printf(TEXT("%s%d"), *CurrentComboData->MontageSectionNamePrefix, CurrentCombo);
	return NextSection;
}

void UGA_CommaBowAttack::StartComboTimer()
{
	int32 ComboIndex = CurrentCombo - 1;
	ensure(CurrentComboData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	const float ComboEffectiveTime = CurrentComboData->EffectiveFrameCount[ComboIndex] / CurrentComboData->FrameRate;
	if (ComboEffectiveTime > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &UGA_CommaBowAttack::CheckComboInput, ComboEffectiveTime, false);
	}
}

void UGA_CommaBowAttack::CheckComboInput()
{
	ComboTimerHandle.Invalidate();
	if (HasNextComboInput)
	{
		MontageJumpToSection(GetNextSection());
		StartComboTimer();
		HasNextComboInput = false;
	}
}
