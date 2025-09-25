// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_SpawnProjectile.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Prologue/Prologue.h"
#include "Prologue/Controller/PrologueAIController.h"
#include "Prologue/Weapon/Projectile/EggBallProjectile.h"

UGA_SpawnProjectile::UGA_SpawnProjectile()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_SpawnProjectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ProjectileClass || SpawnSocketName.IsNone())
	{
		LOG_SCREEN_R("Can't find Projectile or SocketName is not set");
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

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

	ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	USkeletalMeshComponent* AvatarMesh = AvatarCharacter->GetMesh();
	if (!AvatarMesh)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Socket에서 Projectile 소환
	const FVector SpawnLocation = AvatarMesh->GetSocketLocation(FName(SpawnSocketName));

	// TargetActor의 위치로 발사 방향 지정
	const FVector TargetLocation = TargetActor->GetActorLocation();
	FVector FireDirection = TargetLocation - SpawnLocation;
	FireDirection.Normalize();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetAvatarActorFromActorInfo();
	SpawnParams.Instigator = AvatarCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AEggBallProjectile* SpawnProjectile = GetWorld()->SpawnActor<AEggBallProjectile>(
		ProjectileClass,
		SpawnLocation,
		FireDirection.Rotation(),
		SpawnParams
	);

	if (SpawnProjectile)
	{
		SpawnProjectile->FireInDirection(FireDirection);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
