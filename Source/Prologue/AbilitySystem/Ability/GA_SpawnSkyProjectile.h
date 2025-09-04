// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_SpawnSkyProjectile.generated.h"

class AExplodingMangoProjectile;
class AMangoProjectile;
/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_SpawnSkyProjectile : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_SpawnSkyProjectile();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<AExplodingMangoProjectile> MangoProjectile;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	float SpawnHeightOffset = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile|Spawn")
	int32 SpawnProjectiles = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile|Spawn")
	float RandomSpawnRadius = 300.f;	
};
