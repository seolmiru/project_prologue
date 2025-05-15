// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_SpawnSwitchAttackProjectile.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_SpawnSwitchAttackProjectile : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_SpawnSwitchAttackProjectile();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	FName SocketName;
};
