// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PrologueProjectileBase.generated.h"

class UGameplayEffect;
class UProjectileMovementComponent;
class UNiagaraComponent;
class UBoxComponent;

UCLASS()
class PROLOGUE_API APrologueProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	APrologueProjectileBase();

	// GA_SpawnSkyProjectile을 통해서 Blackboard에서 TargetActor를 가져오기 위한 변수
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Projectile|Target")
	TObjectPtr<AActor> TargetActor;
	
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UBoxComponent> ProjectileCollision;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UNiagaraComponent> ProjectileNiagaraComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile|GAS")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayEffect> ToughnessDamageEffect;
};
