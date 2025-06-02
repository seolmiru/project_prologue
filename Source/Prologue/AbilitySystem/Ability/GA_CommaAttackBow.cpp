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
	
	InitializePerfectShotTimer();
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

	GetWorld()->GetTimerManager().SetTimer(CurrentComboTimerHandle, this, &UGA_CommaAttackBow::ResetComboCount, 1.2f, false);

	// 마지막 콤보 실행 직후 교체 공격 Effect 부여
	if (CurrentComboData && CurrentCombo == CurrentComboData->MaxComboCount)
	{
		FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle EffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(SwitchAttackEffectClass, 0.0f, EffectContextHandle);
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}

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

void UGA_CommaAttackBow::StartDebugTimer()
{
	GetWorld()->GetTimerManager().SetTimer(DebugTimerHandle, this, &UGA_CommaAttackBow::DebugTimerInfo, 0.1f, true);
}

void UGA_CommaAttackBow::DebugTimerInfo()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float ElapsedTime = CurrentTime - PerfectShotStartWorldTime;

	float TimeToTagAdd = FMath::Max(0.f, PerfectShotStartTime - ElapsedTime);
	float TimeToTagRemove = FMath::Max(0.f, PerfectShotDuration - ElapsedTime);

	bool bHasPerfectShotTag = false;
	if (GetAbilitySystemComponentFromActorInfo() && PerfectShotRequiredTag.IsValid())
	{
		bHasPerfectShotTag = GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(PerfectShotRequiredTag);
	}

	LOG_SCREEN("Perfect Shot Debug : \n Elapsed : %.1fs \n Time To Tag Add : %1.fs \n Time To Tag Remove : %1.fs \n Tag Active : %s \n PerfectShot Active : %s",
		ElapsedTime,
		TimeToTagAdd,
		TimeToTagRemove,
		bHasPerfectShotTag ? TEXT("YES") : TEXT("NO"),
		bIsPerfectShotActive ? TEXT("YES") : TEXT("NO")
	);

	if (ElapsedTime >= PerfectShotDuration)
	{
		GetWorld()->GetTimerManager().ClearTimer(DebugTimerHandle);
	}
}

void UGA_CommaAttackBow::SyncPerfectShotTag()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	bool bHasTag = ASC->HasMatchingGameplayTag(PerfectShotRequiredTag);

	if (bIsPerfectShotActive && !bHasTag)
	{
		ASC->AddLooseGameplayTag(PerfectShotRequiredTag);
	}
	else if (!bIsPerfectShotActive && bHasTag)
	{
		ASC->RemoveLooseGameplayTag(PerfectShotRequiredTag);
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
	
	if (CurrentCombo >= CurrentComboData->MaxComboCount)
	{
		return;
	}

	MontageJumpToSection(GetNextSection());
	StartComboTimer();
	HasNextComboInput = false;
}

void UGA_CommaAttackBow::InitializePerfectShotTimer()
{
	ClearPerfectShotTimers();

	SyncPerfectShotTag();

	PerfectShotStartWorldTime = GetWorld()->GetTimeSeconds();

	// 활 강공격 활성화 타이머
	GetWorld()->GetTimerManager().SetTimer(AddPerfectShotTagTimerHandle, this, &UGA_CommaAttackBow::HandleAddPerfectShotTag, PerfectShotStartTime, false);

	// 비활성화 타이머
	GetWorld()->GetTimerManager().SetTimer(RemovePerfectShotTagTimerHandle, this, &UGA_CommaAttackBow::HandleRemovePerfectShotTag, PerfectShotDuration, false);

	LOG_SCREEN("PerfectShot Timer Start Time : %.1fs, Duration : %.1fs", PerfectShotStartTime, PerfectShotDuration);
}

void UGA_CommaAttackBow::ClearPerfectShotTimers()
{
	GetWorld()->GetTimerManager().ClearTimer(AddPerfectShotTagTimerHandle);

	GetWorld()->GetTimerManager().ClearTimer(RemovePerfectShotTagTimerHandle);

	if (GetAbilitySystemComponentFromActorInfo() && PerfectShotRequiredTag.IsValid())
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(PerfectShotRequiredTag);
	}

	bIsPerfectShotActive = false;
}

void UGA_CommaAttackBow::HandleAddPerfectShotTag()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	if (!ASC || !PerfectShotRequiredTag.IsValid())
	{
		LOG_SCREEN_R("CommaAttackBow : ASC or Tag invalid");
		bIsPerfectShotActive = false;
		return;
	}

	bool bHasTagBefore = ASC->HasMatchingGameplayTag(PerfectShotRequiredTag);

	if (!bHasTagBefore)
	{
		ASC->AddLooseGameplayTag(PerfectShotRequiredTag);
		bIsPerfectShotActive = true;
	}
	else
	{
		bIsPerfectShotActive = true;
	}
}

void UGA_CommaAttackBow::HandleRemovePerfectShotTag()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	if (!ASC || !PerfectShotRequiredTag.IsValid())
	{
		LOG_SCREEN_R("CommaAttackBow : ASC or Tag invalid");
		bIsPerfectShotActive = false;
		return;
	}


	bool bHadTagBefore = ASC->HasMatchingGameplayTag(PerfectShotRequiredTag);

	if (bHadTagBefore)
	{
		ASC->RemoveLooseGameplayTag(PerfectShotRequiredTag);
	}
	
	bIsPerfectShotActive = false;
}
