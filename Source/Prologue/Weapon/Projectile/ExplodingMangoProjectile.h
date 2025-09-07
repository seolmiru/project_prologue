// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplodingMangoProjectile.generated.h"

class UProjectileMovementComponent;
class UNiagaraComponent;
class UBoxComponent;
class USphereComponent;
class UGameplayEffect;
class UNiagaraSystem;

UCLASS()
class PROLOGUE_API AExplodingMangoProjectile : public AActor
{
	GENERATED_BODY()

public:
	AExplodingMangoProjectile();

	// GA_SpawnSkyProjectile을 통해서 Blackboard에서 TargetActor를 가져오기 위한 변수
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Explosion|Target")
	TObjectPtr<AActor> TargetActor;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	virtual void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void StickAndExplosion(const FHitResult& Hit);
	
	void Explode();

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	TObjectPtr<UBoxComponent> ProjectileCollision;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	TObjectPtr<UNiagaraComponent> ProjectileNiagaraComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	
	float ElapsedTime = 0.f;

	FTimerHandle ExplosionTimerHandle;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion")
	float ExplosionRadius = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion")
	float TimeToExplode = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion")
	bool bShowDebug = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Effect")
	TObjectPtr<UNiagaraSystem> ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Effect")
	TObjectPtr<USoundBase> ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|GAS")
	TSubclassOf<UGameplayEffect> DamageEffect;
};
