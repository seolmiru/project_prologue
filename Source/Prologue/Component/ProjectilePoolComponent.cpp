// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectilePoolComponent.h"


UProjectilePoolComponent::UProjectilePoolComponent()
{
	ProjectilePool = nullptr;
}

AExplodingMangoProjectile* UProjectilePoolComponent::Pop()
{
	if (ProjectilePool)
	{
		return ProjectilePool->Pop();
	}

	return nullptr;
}

void UProjectilePoolComponent::Return(AExplodingMangoProjectile* Projectile)
{
	if (ProjectilePool)
	{
		ProjectilePool->Return(Projectile);
	}
}

void UProjectilePoolComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ProjectilePool)
	{
		delete ProjectilePool;
		ProjectilePool = nullptr;
	}
	
	Super::EndPlay(EndPlayReason);
}


void UProjectilePoolComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!ProjectilePool && GetWorld() && MangoProjectileClass)
	{
		if (!ProjectilePool)
		{
			FActorSpawnParameters SpawnParam;
			SpawnParam.Owner = GetOwner();
			SpawnParam.Instigator = Cast<APawn>(GetOwner());
			SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
			ProjectilePool = new Pool<AExplodingMangoProjectile>(GetWorld(), MangoProjectileClass, 32, SpawnParam);
		}
	}
}
