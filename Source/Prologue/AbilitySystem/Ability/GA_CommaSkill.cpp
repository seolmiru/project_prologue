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
	
	AComma* Comma = Cast<AComma>(ActorInfo->AvatarActor.Get());

	if (Comma)
	{
		Comma->GetDashCollision()->SetActive(true);
		Comma->GetDashCollision()->OnComponentBeginOverlap.AddDynamic(this, &UGA_CommaSkill::OnOverlap);

		TArray<AActor*> OverlappingActors;
		Comma->GetDashCollision()->GetOverlappingActors(OverlappingActors);

		for (AActor* OverlappingActor : OverlappingActors)
		{
			HandleTargetHit(OverlappingActor, FHitResult());
		}
	}
	
	Comma->RotateToMouse();

	Comma->GetDashPoint()->SetCursorDirectionState(false);
	
	// Invincible Effect
	FGameplayEffectContextHandle InvincibleEffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	InvincibleEffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle InvincibleEffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(InvincibleEffectClass, 0.f, InvincibleEffectContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*InvincibleEffectSpecHandle.Data.Get());
	
	BasePos = GetAvatarActorFromActorInfo()->GetActorLocation();
	TargetPos = Comma->GetDashPoint()->GetSkillPoint();

	// Z축 보정
	TargetPos.Z += Comma->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	
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
	bHitStopApplied = false;
	
	AComma* Comma = Cast<AComma>(ActorInfo->AvatarActor.Get());

	if (Comma)
	{
		Comma->GetDashCollision()->SetActive(false);
		Comma->GetDashCollision()->OnComponentBeginOverlap.Clear();
	}

	Comma->GetDashPoint()->SetCursorDirectionState(true);

	Comma->GetCharacterMovement()->UpdateFloorFromAdjustment();
	
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_CommaSkill::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	bHitStopApplied = false;
	
	AComma* Comma = Cast<AComma>(ActorInfo->AvatarActor.Get());

	if (Comma)
	{
		Comma->GetDashCollision()->SetActive(false);
		Comma->GetDashCollision()->OnComponentBeginOverlap.Clear();
	}

	Comma->GetDashPoint()->SetCursorDirectionState(true);

	Comma->GetCharacterMovement()->UpdateFloorFromAdjustment();
	
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

void UGA_CommaSkill::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HandleTargetHit(OtherActor, SweepResult);
}

void UGA_CommaSkill::HandleTargetHit(AActor* TargetActor, const FHitResult& SweepResult)
{
	bool bHitDetected = false;

	bool bHitNormalTarget = false;
	
	APrologueEnemyCharacter* Enemy = Cast<APrologueEnemyCharacter>(TargetActor);
	if (!Enemy || HitActors.Contains(TargetActor))
	{
		return;
	}
	
	LOG_SCREEN("%s", *TargetActor->GetName());

	FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	EffectContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());

	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	
	if (SourceASC && TargetASC)
	{
		if (!TargetASC->HasMatchingGameplayTag(PrologueGameplayTags::Shared_State_NoHitEffect))
		{
			bHitNormalTarget = true;
		}

		bHitDetected = true;
		
		// 대미지 적용
		FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(SkillDamageEffect, 1.f, EffectContextHandle);
		FGameplayAbilityTargetDataHandle DataHandle;
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(SweepResult);
		DataHandle.Add(TargetData);
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		if (!TargetASC->HasMatchingGameplayTag(PrologueGameplayTags::Shared_State_NoHitEffect))
		{
			// 경직 적용
			FGameplayEventData PlayData;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, PrologueGameplayTags::Enemy_Event_Stun, PlayData);

			// 피격 이펙트, 사운드 출력
			TargetASC->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_EnemySkillHit);
			TargetASC->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_SkillDamagingSound);

			// 히트스탑 적용
			if (!bHitStopApplied)
			{
				HitStop();
				bHitStopApplied = true;
			}

			// 카메라 쉐이킹 적용
			if (bHitNormalTarget)
			{
				GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_Damaging);
			}
		}
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
