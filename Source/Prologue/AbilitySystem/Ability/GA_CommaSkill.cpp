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

	bHitStopApplied = false;
	
	AComma* Comma = Cast<AComma>(ActorInfo->AvatarActor.Get());

	if (UCapsuleComponent* CapsuleComponent = Comma->GetCapsuleComponent())
	{
		CapsuleComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
	}

	if (UCharacterMovementComponent* MovementComp = Comma->GetCharacterMovement())
	{
		MovementComp->bEnablePhysicsInteraction = false;

		MovementComp->bPushForceUsingZOffset = false;
	}

	if (Comma)
	{
		Comma->GetParryCollision()->SetActive(true);
		Comma->GetParryCollision()->OnComponentBeginOverlap.AddDynamic(this, &UGA_CommaSkill::OnOverlap);
	}
	
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
	
	// 투세차 반사 확정 전까지 미사용
	//Deflect(Comma);
}

void UGA_CommaSkill::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
}

void UGA_CommaSkill::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_CommaSkill::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	bHitStopApplied = false;
	
	AComma* Comma = Cast<AComma>(ActorInfo->AvatarActor.Get());

	Comma->GetDashPoint()->SetCursorDirectionState(true);

	if (UCapsuleComponent* CapsuleComp = Comma->GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
	}

	if (UCharacterMovementComponent* MovementComp = Comma->GetCharacterMovement())
	{
		MovementComp->bEnablePhysicsInteraction = true;

		MovementComp->bPushForceUsingZOffset = true;
	}

	if (ActorInfo && ActorInfo->AvatarActor.Get())
	{
		Comma->GetParryCollision()->SetActive(false);
		Comma->GetParryCollision()->OnComponentBeginOverlap.Clear();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_CommaSkill::OnDashCurveTick(float Alpha)
{
	FVector CurrentPos = FMath::Lerp(BasePos, TargetPos, Alpha);

	if (AComma* Comma = Cast<AComma>(GetAvatarActorFromActorInfo()))
	{
		FHitResult HitResult;
		FVector TraceStart = CurrentPos + FVector(0.f, 0.f, 100.f);
		FVector TraceEnd = CurrentPos - FVector(0.f, 0.f, 200.f);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Comma);

		if (GetWorld()->LineTraceSingleByChannel(
			HitResult,
			TraceStart,
			TraceEnd,
			ECC_WorldStatic,
			QueryParams
		))
		{
			CurrentPos.Z = HitResult.ImpactPoint.Z + Comma->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		}
	}

	GetAvatarActorFromActorInfo()->SetActorLocation(CurrentPos);
}

void UGA_CommaSkill::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Collision에 Overlap된 적이 있다면
	if (APrologueEnemyCharacter* Enemy = Cast<APrologueEnemyCharacter>(OtherActor))
	{
		LOG_SCREEN("%s", *OtherActor->GetName());

		// Overlap된 대상 Actor 가져오기
		if (SweepResult.GetActor())
		{
			FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
			EffectContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
			
			UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SweepResult.GetActor());
			
			// 대미지 적용
			FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(SkillDamageEffect, 1.f, EffectContextHandle);
			FGameplayAbilityTargetDataHandle DataHandle;
			FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(SweepResult);
			DataHandle.Add(TargetData);
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle, DataHandle);

			// 경직 적용
			FGameplayEventData PlayData;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SweepResult.GetActor(), PrologueGameplayTags::Enemy_Event_Stun, PlayData);
			
			// 피격 이펙트, 사운드 출력
			TargetASC->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_EnemySkillHit);
			SourceASC->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_SkillDamagingSound);

			// 히트스탑 적용
			if (!bHitStopApplied)
			{
				HitStop();
				bHitStopApplied = true;
			}
		}
	}
}

// 투사체 반사 함수
void UGA_CommaSkill::Deflect(AComma* Comma)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Comma);

	// ParryRadius 범위 내에 투사체 탐지
	TArray<AActor*> OverlappedActors;
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		Comma->GetActorLocation(),
		DeflectRadius,
		ObjectTypes,
		ABazierProjectile::StaticClass(),
		IgnoreActors,
		OverlappedActors
	);
	
	for (AActor* Actor : OverlappedActors)
	{
		if (ABazierProjectile* Projectile = Cast<ABazierProjectile>(Actor))
		{
			Projectile->Deflected(Comma);

			GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_Stun);
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
