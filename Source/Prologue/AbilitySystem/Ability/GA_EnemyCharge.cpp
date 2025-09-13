// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_EnemyCharge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/Character/Enemy/PrologueEnemyCharacter.h"
#include "Prologue/Character/Player/Comma.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

void UGA_EnemyCharge::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	HitActors.Empty();
	
	UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, OverlapCheckDelay);
	DelayTask->OnFinish.AddDynamic(this, &UGA_EnemyCharge::OnDelayFinished);

	DelayTask->ReadyForActivation();
}

void UGA_EnemyCharge::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_EnemyCharge::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	APrologueEnemyCharacter* EnemyCharacter = Cast<APrologueEnemyCharacter>(GetAvatarActorFromActorInfo());
	if (!EnemyCharacter)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UCapsuleComponent* CapsuleComponent = EnemyCharacter->GetCapsuleComponent();

	CapsuleComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	
	CapsuleComponent->OnComponentBeginOverlap.Clear();
}

void UGA_EnemyCharge::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HandleTargetHit(OtherActor, SweepResult);
}

void UGA_EnemyCharge::HandleTargetHit(AActor* TargetActor, const FHitResult& SweepResult)
{
	if (HitActors.Contains(TargetActor))
	{
		return;
	}
	
	AComma* Comma = Cast<AComma>(TargetActor);
	if (!Comma)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	EffectContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());

	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (SourceASC && TargetASC)
	{
		FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(ChargeDamageEffect, 1.f, EffectContextHandle);
		FGameplayAbilityTargetDataHandle DataHandle;
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(SweepResult);
		DataHandle.Add(TargetData);
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		HitActors.Add(TargetActor);
		
		TargetASC->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_PlayerHit);
	}
}

void UGA_EnemyCharge::OnDelayFinished()
{
	APrologueEnemyCharacter* EnemyCharacter = Cast<APrologueEnemyCharacter>(GetAvatarActorFromActorInfo());
	if (!EnemyCharacter)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	UCapsuleComponent* CapsuleComponent = EnemyCharacter->GetCapsuleComponent();
	
	CapsuleComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &UGA_EnemyCharge::OnOverlap);

	TArray<AActor*> OverlappingActors;
	CapsuleComponent->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		HandleTargetHit(OverlappingActor, FHitResult());
	}
}
