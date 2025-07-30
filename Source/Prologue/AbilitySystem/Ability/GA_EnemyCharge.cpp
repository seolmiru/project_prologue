// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_EnemyCharge.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AT/AT_TickCurve.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UGA_EnemyCharge::UGA_EnemyCharge()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_EnemyCharge::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	FVector StartPos = GetAvatarActorFromActorInfo()->GetActorLocation();
	FVector EndPos = StartPos + GetAvatarActorFromActorInfo()->GetActorForwardVector() * MoveLength;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetAvatarActorFromActorInfo());
	FHitResult Hit;

	bool bResult = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		StartPos,
		EndPos,
		UEngineTypes::ConvertToTraceType(ECC_EngineTraceChannel3),
		false,
		IgnoreActors,
		EDrawDebugTrace::ForDuration,
		Hit,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		2.f
	);

	BasePos = GetAvatarActorFromActorInfo()->GetActorLocation();
	TargetPos = bResult ? Hit.ImpactPoint : EndPos;
	
	UAbilityTask_PlayMontageAndWait* PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), AnimMontage, 1.0f);
	PlayTask->OnCompleted.AddDynamic(this, &UGA_EnemyCharge::OnComplete);
	PlayTask->OnInterrupted.AddDynamic(this, &UGA_EnemyCharge::OnInterrupted);
	PlayTask->ReadyForActivation();

	UAT_TickCurve* TickCurve = UAT_TickCurve::CreateTask(this, ChargeCurve);
	TickCurve->OnCurveTick.AddDynamic(this, &UGA_EnemyCharge::OnCurveTick);
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
}

void UGA_EnemyCharge::OnCurveTick(float Alpha)
{
	GetAvatarActorFromActorInfo()->SetActorLocation(FMath::Lerp(BasePos, TargetPos, Alpha));
}

void UGA_EnemyCharge::OnComplete()
{
	bool bReplicatedEndAbility = true; 
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UGA_EnemyCharge::OnInterrupted()
{
	bool bReplicatedEndAbility = true; 
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
