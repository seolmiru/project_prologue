// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prologue/Weapon/Projectile/ExplodingMangoProjectile.h"
#include "ProjectilePoolComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROLOGUE_API UProjectilePoolComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UProjectilePoolComponent();

	AExplodingMangoProjectile* Pop();

	void Return(AExplodingMangoProjectile* Projectile);
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, Category = "Pool")
	TSubclassOf<AExplodingMangoProjectile> MangoProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Pool")
	int32 PoolSize = 32;	

private:
	Pool<AExplodingMangoProjectile>* ProjectilePool;
};
