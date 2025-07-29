// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CommaParry.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AT/AT_TickCurve.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/Character/Player/Comma.h"
#include "Prologue/Character/Player/PlayerDashPoint.h"
#include "Prologue/Weapon/Projectile/BazierProjectile.h"

void UGA_CommaParry::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AComma* Comma = Cast<AComma>(ActorInfo->AvatarActor.Get());

	Comma->RotateToMouse();

	Comma->GetDashPoint()->SetCursorDirectionState(false);

	if (UCapsuleComponent* CapsuleComp = Comma->GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Overlap);
	}

	// 패링 성공 이벤트 대기
	UAbilityTask_WaitGameplayEvent* WaitJustParryTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, PrologueGameplayTags::Comma_Event_JustParry);
	WaitJustParryTask->EventReceived.AddDynamic(this, &UGA_CommaParry::OnJustParry);
	WaitJustParryTask->ReadyForActivation();
	
	// Parry Effect
	FGameplayEffectContextHandle ParryEffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ParryEffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle ParryEffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(ParryEffectClass, 0.f, ParryEffectContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*ParryEffectSpecHandle.Data.Get());

	// Invincible Effect
	FGameplayEffectContextHandle InvincibleEffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	InvincibleEffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle InvincibleEffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(ParryEffectClass, 0.f, InvincibleEffectContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*InvincibleEffectSpecHandle.Data.Get());

	// Dash Tick Curve Task
	UAT_TickCurve* DashTickCurve = UAT_TickCurve::CreateTask(this, DashCurve);
	DashTickCurve->OnCurveTick.AddDynamic(this, &UGA_CommaParry::OnDashCurveTick);
	DashTickCurve->OnComplete.AddDynamic(this, &UGA_CommaParry::OnComplete);
	
	BasePos = GetAvatarActorFromActorInfo()->GetActorLocation();
	TargetPos = Comma->GetDashPoint()->GetParryPoint();
	
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetAvatarActorFromActorInfo());
	TArray<FHitResult> Hits;

	bool bResult = UKismetSystemLibrary::LineTraceMulti(
		GetWorld(),
		BasePos,
		TargetPos,
		UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4),
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		Hits,
		true
	);
	
	// 대상이 공격 중인지 체크
	if (bResult && Hits.Num() > 0)
	{
		for (const FHitResult& Hit : Hits)
		{
			if (AActor* HitActor = Hit.GetActor())
			{
				if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
				{
					// 대상이 공격 중이라면 대상 앞까지만 돌진
					if (TargetASC->HasMatchingGameplayTag(PrologueGameplayTags::Shared_State_IsAttacking))
					{
						TargetPos = Hit.ImpactPoint;
						break;
					}
				}
			}
		}
	}

	DashTickCurve->ReadyForActivation();
	
	// 투세차 반사 확정 전까지 미사용
	//Deflect(Comma);
}

void UGA_CommaParry::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
}

void UGA_CommaParry::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_CommaParry::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	AComma* Comma = Cast<AComma>(ActorInfo->AvatarActor.Get());

	Comma->GetDashPoint()->SetCursorDirectionState(true);

	if (UCapsuleComponent* CapsuleComp = Comma->GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Block);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_CommaParry::OnDashCurveTick(float Alpha)
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

void UGA_CommaParry::OnComplete()
{
	if (AComma* Comma = Cast<AComma>(GetAvatarActorFromActorInfo()))
	{
		if (UCharacterMovementComponent* MovementComp = Comma->GetCharacterMovement())
		{
			MovementComp->UpdateFloorFromAdjustment();
		}
	}
	
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UGA_CommaParry::OnJustParry(FGameplayEventData Payload)
{
	FGameplayTagContainer CooldownTags;

	if (CooldownTags.Num() > 0)
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		ASC->RemoveActiveEffectsWithGrantedTags(CooldownTags);
	}
}

// 투사체 반사 함수
void UGA_CommaParry::Deflect(AComma* Comma)
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
		ParryRadius,
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

			GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_Parried);
		}
	}
}
