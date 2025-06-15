// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CommaAttackBow.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/Character/Enemy/PrologueEnemyCharacter.h"
#include "Prologue/Character/Player/Comma.h"
#include "Prologue/DataAsset/ComboBowData.h"

UGA_CommaAttackBow::UGA_CommaAttackBow()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_CommaAttackBow::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return !ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Comma.State.SwitchAttack.Bow")));
}

void UGA_CommaAttackBow::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AComma* Comma = CastChecked<AComma>(ActorInfo->AvatarActor.Get());

	CurrentComboData = Comma->GetComboBowData();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->AddLooseGameplayTag(PrologueGameplayTags::Comma_State_CancelDisabled);
		ASC->RemoveLooseGameplayTag(PrologueGameplayTags::Comma_State_CancelEnabled);
	}

	if (CurrentComboData && CurrentCombo >= CurrentComboData->MaxComboCount)
	{
		ResetComboCount();
	}

	// SphereRadius 범위 내의 모든 적 감지
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Comma);

	TArray<AActor*> OverlappedActors;
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		Comma->GetActorLocation(),
		SphereRadius,
		ObjectTypes,
		APrologueEnemyCharacter::StaticClass(),
		IgnoreActors,
		OverlappedActors
	);

	for (AActor* Actor : OverlappedActors)
	{
		DrawDebugPoint(
			GetWorld(),
			Actor->GetActorLocation(),
			12.f,
			FColor::Red,
			false,
			2.0f
		);
	}

	// 감지된 적들 중에 가장 가까운 적을 타겟으로 지정
	float NearestDist = TNumericLimits<float>::Max();
	TargetActor = nullptr;
	for (AActor* Actor : OverlappedActors)
	{
		float Dist = FVector::DistSquared(Actor->GetActorLocation(), Comma->GetActorLocation());
		if (Dist < NearestDist)
		{
			NearestDist = Dist;
			TargetActor = Actor;
		}
	}

	// TargetActor가 있다면 그 방향으로 회전, 없다면 마우스 방향에 의존
	if (TargetActor)
	{
		Comma->RotateToTarget(TargetActor);
	}
	else
	{
		Comma->RotateToMouse();
	}
	
	Comma->GetSwordWeaponMesh()->SetVisibility(false);
	Comma->GetBowWeaponMesh()->SetVisibility(true);

	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayAttack"), Comma->GetBowComboMontage(), 1.0f, GetNextSection());
	PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_CommaAttackBow::OnComplete);
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_CommaAttackBow::OnInterrupted);
	PlayAttackTask->OnBlendOut.AddDynamic(this, &UGA_CommaAttackBow::OnBlendOut);
	PlayAttackTask->ReadyForActivation();
	
	
	GetWorld()->GetTimerManager().ClearTimer(CurrentComboTimerHandle);

	StartComboTimer();
}

void UGA_CommaAttackBow::InputPressed(const FGameplayAbilitySpecHandle Handle,
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

void UGA_CommaAttackBow::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	CurrentComboData = nullptr;
	HasNextComboInput = false;
}

void UGA_CommaAttackBow::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UAbilitySystemComponent* ASC =GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(PrologueGameplayTags::Comma_State_CancelDisabled);
		ASC->RemoveLooseGameplayTag(PrologueGameplayTags::Comma_State_CancelEnabled);
	}
	
	GetWorld()->GetTimerManager().SetTimer(CurrentComboTimerHandle, this, &UGA_CommaAttackBow::ResetComboCount, 1.2f, false);

	CurrentComboData = nullptr;
	HasNextComboInput = false;
}

void UGA_CommaAttackBow::OnComplete()
{
	if (!HasNextComboInput && !GetWorld()->GetTimerManager().IsTimerActive(ComboTimerHandle))
	{
		bool bReplicatedEndAbility = true;
		bool bWasCancelled = false;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
	}
}

void UGA_CommaAttackBow::OnInterrupted()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UGA_CommaAttackBow::OnBlendOut()
{
	if (!HasNextComboInput && !GetWorld()->GetTimerManager().IsTimerActive(ComboTimerHandle))
	{
		bool bReplicatedEndAbility = true;
		bool bWasCancelled = false;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
	}
}

FName UGA_CommaAttackBow::GetNextSection()
{
	LOG_SCREEN_R("GetNextSection - Before : %d", CurrentCombo);
	
	CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, CurrentComboData->MaxComboCount);
	FName NextSection = *FString::Printf(TEXT("%s%d"), *CurrentComboData->MontageSectionNamePrefix, CurrentCombo);
	return NextSection;
}

void UGA_CommaAttackBow::StartComboTimer()
{
	int32 ComboIndex = CurrentCombo - 1;
	ensure(CurrentComboData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	// 프레임 수를 시간으로 변환
	const float ComboEffectiveTime = CurrentComboData->EffectiveFrameCount[ComboIndex] / CurrentComboData->FrameRate;

	// 유효 시간이 있다면 타이머 설정, 없으면 즉시 콤보 입력 허용
	if (ComboEffectiveTime > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &UGA_CommaAttackBow::CheckComboInput, ComboEffectiveTime, false);
	}
	else
	{
		EnableComboInput();
	}
}

void UGA_CommaAttackBow::CheckComboInput()
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

		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		if (ASC)
		{
			ASC->AddLooseGameplayTag(PrologueGameplayTags::Comma_State_CancelDisabled);
			ASC->RemoveLooseGameplayTag(PrologueGameplayTags::Comma_State_CancelEnabled);
		}
		
		MontageJumpToSection(GetNextSection());
		StartComboTimer();
		HasNextComboInput = false;
	}
}

void UGA_CommaAttackBow::ResetComboCount()
{
	CurrentCombo = 0;
	LOG_SCREEN_R("AttackBow : Reset Combo Count");
}

void UGA_CommaAttackBow::EnableComboInput()
{
	ComboTimerHandle.Invalidate();

	if (HasNextComboInput)
	{
		ProcessNextCombo();
	}
}

void UGA_CommaAttackBow::ProcessNextCombo()
{
	LOG_SCREEN_R("ProcessNextCombo - Current : %d", CurrentCombo);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->AddLooseGameplayTag(PrologueGameplayTags::Comma_State_CancelDisabled);
		ASC->RemoveLooseGameplayTag(PrologueGameplayTags::Comma_State_CancelEnabled);
	}
	
	if (CurrentCombo >= CurrentComboData->MaxComboCount)
	{
		return;
	}

	MontageJumpToSection(GetNextSection());
	StartComboTimer();
	HasNextComboInput = false;
}