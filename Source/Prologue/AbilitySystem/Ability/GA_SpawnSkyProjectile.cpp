// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_SpawnSkyProjectile.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Prologue/Controller/PrologueAIController.h"
#include "Prologue/Weapon/Projectile/ExplodingMangoProjectile.h"

UGA_SpawnSkyProjectile::UGA_SpawnSkyProjectile()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_SpawnSkyProjectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	APrologueAIController* AIController = Cast<APrologueAIController>(ActorInfo->OwnerActor->GetInstigatorController());
	if (!AIController)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Blackboard에서 TargetActor 가져오기
	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
	AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(FName("TargetActor")));

	if (!TargetActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	const FVector TargetLocation = TargetActor->GetActorLocation();
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetAvatarActorFromActorInfo();
	SpawnParams.Instigator = Cast<APawn>(GetAvatarActorFromActorInfo());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// SpawnProjectiles 수만큼 투사체 소환
	for (int32 i = 0; i < SpawnProjectiles; ++i)
	{
		// 플레이어 주변에 랜덤하게 투사체 소환
		FVector2D RandomOffset2D = FMath::RandPointInCircle(RandomSpawnRadius);
		FVector RandomOffset = FVector(RandomOffset2D.X, RandomOffset2D.Y, 0.f);

		const FVector SpawnLocation = TargetLocation + RandomOffset + FVector(0.f, 0.f, SpawnHeightOffset);
		const FRotator SpawnRotation = FRotator::ZeroRotator;

		AExplodingMangoProjectile* Projectile = GetWorld()->SpawnActorDeferred<AExplodingMangoProjectile>(
			MangoProjectile,
			FTransform(SpawnRotation, SpawnLocation),
			SpawnParams.Owner,
			SpawnParams.Instigator,
			SpawnParams.SpawnCollisionHandlingOverride
		);

		if (Projectile)
		{
			Projectile->TargetActor = TargetActor;

			Projectile->FinishSpawning(FTransform(SpawnRotation, SpawnLocation));
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
