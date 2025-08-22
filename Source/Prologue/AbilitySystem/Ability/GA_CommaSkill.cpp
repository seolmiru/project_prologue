// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CommaSkill.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AT/AT_TickCurve.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/Character/Enemy/PrologueEnemyCharacter.h"
#include "Prologue/Character/Player/Comma.h"
#include "Prologue/Character/Player/PlayerDashPoint.h"
#include "Prologue/Weapon/Projectile/BazierProjectile.h"

void UGA_CommaSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	HitActors.Reset();
	bHitStopApplied = false;
	
	AComma* Comma = Cast<AComma>(ActorInfo->AvatarActor.Get());
	
	Comma->RotateToMouse();

	Comma->GetDashPoint()->SetCursorDirectionState(false);
	
	// Invincible Effect
	FGameplayEffectContextHandle InvincibleEffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	InvincibleEffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle InvincibleEffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(InvincibleEffectClass, 0.f, InvincibleEffectContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*InvincibleEffectSpecHandle.Data.Get());
	
	BasePos = GetAvatarActorFromActorInfo()->GetActorLocation();
	TargetPos = Comma->GetDashPoint()->GetParryPoint();
	
	// Dash Tick Curve Task
	UAT_TickCurve* DashTickCurve = UAT_TickCurve::CreateTask(this, DashCurve);
	DashTickCurve->OnCurveTick.AddDynamic(this, &UGA_CommaSkill::OnDashCurveTick);
	DashTickCurve->ReadyForActivation();
}

void UGA_CommaSkill::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
}

void UGA_CommaSkill::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	EndHitStop();
	
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_CommaSkill::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	bHitStopApplied = false;
	
	AComma* Comma = Cast<AComma>(ActorInfo->AvatarActor.Get());

	Comma->GetDashPoint()->SetCursorDirectionState(true);
	
	EndHitStop();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_CommaSkill::OnDashCurveTick(float Alpha)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor)
	{
		FVector InterpolatedLocation = FMath::Lerp(BasePos, TargetPos, Alpha);
		AvatarActor->SetActorLocation(InterpolatedLocation);
	}
}

void UGA_CommaSkill::HitStop()
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
		&UGA_CommaSkill::EndHitStop,
		HitStopDuration * HitStopTimeScale,
		false
	);
}

void UGA_CommaSkill::EndHitStop()
{
	// 시간 속도 복구
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);

	if (AComma* Comma = Cast<AComma>(GetAvatarActorFromActorInfo()))
	{
		Comma->CustomTimeDilation = 1.f;
	}
}
