// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CommaHitStop.h"

#include "Kismet/GameplayStatics.h"
#include "Prologue/Character/Player/Comma.h"

UGA_CommaHitStop::UGA_CommaHitStop()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void UGA_CommaHitStop::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	HitStop();
}

void UGA_CommaHitStop::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	EndHitStop();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_CommaHitStop::HitStop()
{
	// 기존 히트스탑이 진행 중인 상황이라면 타이머 초기화
	if (GetWorld()->GetTimerManager().IsTimerActive(HitStopTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(HitStopTimerHandle);
	}
	
	// HitStopTimeScale만큼 느려지게 설정
	if (AComma* Comma = Cast<AComma>(GetAvatarActorFromActorInfo()))
	{
		Comma->CustomTimeDilation = HitStopTimeScale;
	}

	// 전체 게임 속도 조절
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), HitStopTimeScale);

	// HitStopTimer가 종료되면 EndHitStop 호출
	GetWorld()->GetTimerManager().SetTimer(
		HitStopTimerHandle,
		this,
		&UGA_CommaHitStop::EndHitStop,
		HitStopDuration * HitStopTimeScale,
		false
	);
}

void UGA_CommaHitStop::EndHitStop()
{
	// 시간 속도 복구
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);

	if (AComma* Comma = Cast<AComma>(GetAvatarActorFromActorInfo()))
	{
		Comma->CustomTimeDilation = 1.f;
	}
}
