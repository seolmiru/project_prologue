// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_OverClock.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/PostProcessComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/AbilitySystem/Attribute/PrologueSkillAttributeSet.h"
#include "Prologue/Character/Enemy/PrologueEnemyCharacter.h"
#include "Prologue/Character/Player/Comma.h"
#include "Prologue/Controller/CommaController.h"
#include "Prologue/Weapon/Projectile/BazierProjectile.h"
#include "Prologue/Weapon/Projectile/PrologueProjectileBase.h"

bool UGA_OverClock::bIsOverClockActive = false;
float UGA_OverClock::OverClockTimeScale = 1.0f;
FOnTimeScale UGA_OverClock::OnTimeScale;

UGA_OverClock::UGA_OverClock()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_OverClock::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	if (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(PrologueGameplayTags::Comma_State_Skill))
	{
		return false;
	}
	
	return true;
}

void UGA_OverClock::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bIsOverClockActive = true;
	OverClockTimeScale = TimeScale;
	CenterLocation = ActorInfo->AvatarActor->GetActorLocation();

	// OverClock 영역 Debug
	DrawDebugBox(
		GetWorld(),
		CenterLocation,
		FVector(Radius, Radius, HalfHeight),
		FQuat::Identity,
		FColor::Cyan,
		false,
		bShowDebug ? OverClockDuration : EDrawDebugTrace::None,
		0,
		3.f
	);

	// OverClock 종료 Timer
	GetWorld()->GetTimerManager().SetTimer(
		OverClockTimerHandle,
		this,
		&UGA_OverClock::OnOverClockFinished,
		OverClockDuration,
		false
	);

	// Tick마다 영역 검사하는 Timer
	GetWorld()->GetTimerManager().SetTimer(
		CheckAreaTimerHandle,
		this,
		&UGA_OverClock::CheckActorsInArea,
		CheckInterval,
		true,
		0.f
	);

	FVector OverClockLocation = GetAvatarActorFromActorInfo()->GetActorLocation();

	FVector StartLocation = OverClockLocation;
	FVector EndLocation = OverClockLocation - FVector(0.f, 0.f, 1000.f);

	FHitResult HitResult;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetAvatarActorFromActorInfo());

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_GameTraceChannel7,
		CollisionParams
	);

	FVector OverClockSpawnLocation = bHit ? HitResult.ImpactPoint : OverClockLocation;
	
	if (OverClockNiagaraSystem)
	{
		UNiagaraComponent* OverClockNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			OverClockNiagaraSystem,
			OverClockSpawnLocation,
			FRotator::ZeroRotator,
			FVector(1.f, 1.f, 1.f),
			true,
			true
		);
	}
}

void UGA_OverClock::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Timer 정리
	GetWorld()->GetTimerManager().ClearTimer(OverClockTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(CheckAreaTimerHandle);
	
	bIsOverClockActive = false;

	// 시간 복구
	RestoreEnemyTime();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, false);
}

void UGA_OverClock::OnOverClockFinished()
{
	LOG_SCREEN_R("End OverClock. Restoring time.");
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_OverClock::CheckActorsInArea()
{
	// 현재 프레임에서 OverClock 영역 내에 있는 Actor 저장
	TSet<TWeakObjectPtr<AActor>> ActorsInAreaNow;
	TArray<AActor*> OverlappingActors;

	// OverClock 영역 검사
	UKismetSystemLibrary::BoxOverlapActors(
		GetWorld(),
		CenterLocation,
		FVector(Radius, Radius, HalfHeight),
		TArray<TEnumAsByte<EObjectTypeQuery>>(),
		AActor::StaticClass(),
		TArray<AActor*>(),
		OverlappingActors
	);

	for (AActor* Actor : OverlappingActors)
	{
		// OverClock의 영향을 받을 대상인지 검사
		if (Cast<APrologueEnemyCharacter>(Actor) || Cast<ABazierProjectile>(Actor) || Cast<APrologueProjectileBase>(Actor))
		{
			// 영향을 받고 있지 않은 새로운 Actor일 때 OverClock 효과 적용
			if (!AffectedActors.Contains(Actor))
			{
				Actor->CustomTimeDilation = TimeScale;
				AffectedActors.Add(Actor);

				// MaterialInstanceDynamic 생성
				TArray<UMaterialInstanceDynamic*> MDIs;
				TArray<UPrimitiveComponent*> PrimitiveComponents;
				Actor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

				for (UPrimitiveComponent* Comp : PrimitiveComponents)
				{
					// OverClock에 영향을 받고 있는 Actor들에게 Material 효과 적용
					if (USkeletalMeshComponent* MeshComp = Cast<USkeletalMeshComponent>(Comp))
					{
						for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
						{
							UMaterialInstanceDynamic* MDI = MeshComp->CreateDynamicMaterialInstance(i, MeshComp->GetMaterial(i));
							if (MDI)
							{
								MDI->SetScalarParameterValue(FName("OverClockFxPower"), 1.f);
								MDIs.Add(MDI);
							}
						}
					}
				}
				if (MDIs.Num() > 0)
				{
					AffectedActorMaterial.Add(Actor, MDIs);
				}
			}

			// 현재 OverClock 영역에 있는 Actor 목록에 추가
			ActorsInAreaNow.Add(Actor);
		}
	}

	// 이전에 영향을 받았지만 OverClock 영역을 벗어난 Actor 찾기
	TSet<TWeakObjectPtr<AActor>> ExitedActors = AffectedActors.Difference(ActorsInAreaNow);
	for (const auto& ActorPtr : ExitedActors)
	{
		if (ActorPtr.IsValid())
		{
			// 시간 복구
			ActorPtr->CustomTimeDilation = 1.f;

			// Material 복구
			if (AffectedActorMaterial.Contains(ActorPtr))
			{
				for (UMaterialInstanceDynamic* MDI : AffectedActorMaterial[ActorPtr])
				{
					if (MDI)
					{
						MDI->SetScalarParameterValue(FName("OverClockFxPower"), 0.f);
					}
				}
				AffectedActorMaterial.Remove(ActorPtr);
			}
		}
		
		AffectedActors.Remove(ActorPtr);
	}
}

// 원래 속도, Material로 복구
void UGA_OverClock::RestoreEnemyTime()
{
	for (const auto& ActorPtr : AffectedActors)
	{
		if (ActorPtr.IsValid())
		{
			ActorPtr->CustomTimeDilation = 1.f;

			if (AffectedActorMaterial.Contains(ActorPtr))
			{
				for (UMaterialInstanceDynamic* MDI : AffectedActorMaterial[ActorPtr])
				{
					if (MDI)
					{
						MDI->SetScalarParameterValue(FName("OverClockFxPower"), 0.f);
					}
				}
			}
		}
	}

	AffectedActors.Empty();
	AffectedActorMaterial.Empty();
}
