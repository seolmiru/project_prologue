// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_SpawnSkyProjectile.h"

#include "Kismet/GameplayStatics.h"
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

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	const FVector TargetLocation = PlayerPawn->GetActorLocation();
	
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
		
		GetWorld()->SpawnActor<AExplodingMangoProjectile>(MangoProjectile, SpawnLocation, SpawnRotation, SpawnParams);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
