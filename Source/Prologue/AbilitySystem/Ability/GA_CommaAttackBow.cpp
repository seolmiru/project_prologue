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

	if (EnableComboInputTag.IsValid())
	{
		EnableComboInputEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EnableComboInputTag);
		if (EnableComboInputEventTask)
		{
			EnableComboInputEventTask->EventReceived.AddDynamic(this, &UGA_CommaAttackBow::HandleEnableComboInputEvent);
			EnableComboInputEventTask->ReadyForActivation();
		}
	}

	if (DisableComboInputTag.IsValid())
	{
		DisableComboInputEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, DisableComboInputTag);
		if (DisableComboInputEventTask)
		{
			DisableComboInputEventTask->EventReceived.AddDynamic(this, &UGA_CommaAttackBow::HandleDisableComboInputEvent);
			DisableComboInputEventTask->ReadyForActivation();
		}
	}

	InitializePerfectShotTimer();

	StartDebugTimer();
}

void UGA_CommaAttackBow::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	
	if (bComboInputActivate)
	{
		GetWorld()->GetTimerManager().ClearTimer(ComboTimerHandle);

		FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle EffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(SwitchAttackEffectClass, 0.0f, EffectContextHandle);
		GetAbilitySystemComponentFromActorInfo()->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
		
		K2_ActivateAbility();
		
		bComboInputActivate = false;
	}
}

void UGA_CommaAttackBow::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_CommaAttackBow::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	if (EffectCount == 3)
	{
		FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle EffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(SwitchAttackEffectClass, 0.0f, EffectContextHandle);
		GetAbilitySystemComponentFromActorInfo()->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
	}
}

void UGA_CommaAttackBow::OnComplete()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UGA_CommaAttackBow::OnInterrupted()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UGA_CommaAttackBow::HandleEnableComboInputEvent(FGameplayEventData Payload)
{
	bComboInputActivate = true;
}

void UGA_CommaAttackBow::HandleDisableComboInputEvent(FGameplayEventData Payload)
{
	bComboInputActivate = false;
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

void UGA_CommaAttackBow::InitializePerfectShotTimer()
{
	ClearPerfectShotTimers();

	PerfectShotStartWorldTime = GetWorld()->GetTimeSeconds();

	GetWorld()->GetTimerManager().SetTimer(AddPerfectShotTagTimerHandle, this, &UGA_CommaAttackBow::HandleAddPerfectShotTag, PerfectShotStartTime, false);

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
	if (GetAbilitySystemComponentFromActorInfo() && PerfectShotRequiredTag.IsValid())
	{
		GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(PerfectShotRequiredTag);
		bIsPerfectShotActive = true;

		LOG_SCREEN_R("PerfectShot On");
	}
}

void UGA_CommaAttackBow::HandleRemovePerfectShotTag()
{
	if (GetAbilitySystemComponentFromActorInfo() && PerfectShotRequiredTag.IsValid())
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(PerfectShotRequiredTag);
	}

	LOG_SCREEN_R("PerfectShot Off");
	
	bIsPerfectShotActive = false;
}
