// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_SpawnSwitchAttackProjectile.h"

#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/Weapon/Projectile/PrologueProjectileBase.h"

UGA_SpawnSwitchAttackProjectile::UGA_SpawnSwitchAttackProjectile()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_SpawnSwitchAttackProjectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* Character = CastChecked<ACharacter>(CurrentActorInfo->AvatarActor.Get());

	USkeletalMeshComponent* MeshComp = Character->GetMesh();

	const FTransform SocketTransform = MeshComp->GetSocketTransform(SocketName, RTS_World);

	APrologueProjectileBase* Projectile = GetWorld()->SpawnActorDeferred<APrologueProjectileBase>(
		ProjectileClass,
		SocketTransform,
		GetAvatarActorFromActorInfo(),
		Cast<APawn>(GetAvatarActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
	);

	if (Projectile)
	{
		if (UProjectileMovementComponent* MoveComp = Projectile->FindComponentByClass<UProjectileMovementComponent>())
		{
			FVector Dir = SocketTransform.GetRotation().Vector();
			MoveComp->Velocity = Dir * MoveComp->InitialSpeed;
		}

		Projectile->SetOwner(GetAvatarActorFromActorInfo());
		Projectile->SetInstigator(Cast<APawn>(GetAvatarActorFromActorInfo()));
		UGameplayStatics::FinishSpawningActor(Projectile, SocketTransform);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
