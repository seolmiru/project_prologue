// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_SpawnProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"

UGA_SpawnProjectile::UGA_SpawnProjectile()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_SpawnProjectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	USkeletalMeshComponent* MeshComp = nullptr;

	if (ACharacter* Character = CastChecked<ACharacter>(CurrentActorInfo->AvatarActor.Get()))
	{
		MeshComp = Character->GetMesh();
	}

	FVector SpawnLocation = MeshComp->GetSocketLocation(SocketName);
	FVector ForwardVec = AvatarActor->GetActorForwardVector();
	FRotator SpawnRotation = FRotationMatrix::MakeFromX(ForwardVec).Rotator();

	FActorSpawnParameters Params;
	Params.Owner = AvatarActor;
	Params.Instigator = Cast<APawn>(AvatarActor);
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* SpawnProjectile = AvatarActor->GetWorld()->SpawnActor<AActor>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		Params
	);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
