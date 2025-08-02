// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_OverClock.h"

#include "NiagaraComponent.h"
#include "Components/PostProcessComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/AbilitySystem/PrologueAttributeSet.h"
#include "Prologue/Character/Enemy/PrologueEnemyCharacter.h"
#include "Prologue/Character/Player/Comma.h"
#include "Prologue/Controller/CommaController.h"
#include "Prologue/Weapon/Projectile/BazierProjectile.h"

bool UGA_OverClock::bIsOverClockActive = false;
float UGA_OverClock::OverClockTimeScale = 1.0f;
FOnTimeScale UGA_OverClock::OnTimeScale;

UGA_OverClock::UGA_OverClock()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_OverClock::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	const UPrologueAttributeSet* AttributeSet = ASC->GetSet<UPrologueAttributeSet>();

	// 오버클락 게이지가 100이 아닐 때에는 시전 불가
	if (AttributeSet->GetCurrentGauge() < AttributeSet->GetMaxGauge())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 게이지 초기화
	ASC->SetNumericAttributeBase(UPrologueAttributeSet::GetCurrentGaugeAttribute(), 0.0f);
	
	bIsOverClockActive = true;
	OverClockTimeScale = TimeScale;
	
	ApplySlowToEnemies();
	
	GetWorld()->GetTimerManager().SetTimer(
		OverClockTimerHandle,
		this,
		&UGA_OverClock::OnOverClockFinished,
		OverClockDuration,
		false
	);
}

void UGA_OverClock::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	GetWorld()->GetTimerManager().ClearTimer(OverClockTimerHandle);

	bIsOverClockActive = false;
	
	RestoreEnemyTime();
}

void UGA_OverClock::OnOverClockFinished()
{
	LOG_SCREEN_R("End OverClock. Restoring time.");
	
	RestoreEnemyTime();
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

// 월드 내에 존재하는 모든 적과 적이 발사한 투사체에 TimeScale만큼 시간이 느려지도록 적용
void UGA_OverClock::ApplySlowToEnemies()
{
	AffectedEnemies.Empty();
	AffectedProjectiles.Empty();

	// EnemyCharacter를 상속 받은 모든 액터 찾기
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		APrologueEnemyCharacter::StaticClass(),
		Found
	);

	for (AActor* Actor : Found)
	{
		if (auto* Enemy = Cast<APrologueEnemyCharacter>(Actor))
		{
			// 시간 배율 적용, AffectedEnemies 목록에 추가
			Enemy->CustomTimeDilation = TimeScale;
			AffectedEnemies.Add(Enemy);
		}
	}

	// BazierProjectile을 상속 받은 모든 액터 찾기
	TArray<AActor*> FoundProjectiles;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		ABazierProjectile::StaticClass(),
		FoundProjectiles
	);

	for (AActor* Actor : FoundProjectiles)
	{
		if (auto* Projectile = Cast<ABazierProjectile>(Actor))
		{
			// 시간 배율 적용, AffectedProjectiles 목록에 추가
			Projectile->CustomTimeDilation = TimeScale;
			AffectedProjectiles.Add(Projectile);
		}
	}
}

// 원래 속도로 복구
void UGA_OverClock::RestoreEnemyTime()
{
	for (auto* Enemy : AffectedEnemies)
	{
		if (IsValid(Enemy))
		{
			Enemy->CustomTimeDilation = 1.0f;
		}
	}

	for (auto* Projectile : AffectedProjectiles)
	{
		if (IsValid(Projectile))
		{
			Projectile->CustomTimeDilation = 1.0f;
		}
	}
	
	AffectedEnemies.Empty();
	AffectedProjectiles.Empty();
}