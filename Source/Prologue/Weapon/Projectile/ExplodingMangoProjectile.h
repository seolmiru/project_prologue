// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrologueProjectileBase.h"
#include "GameFramework/Actor.h"
#include "Prologue/Pool.h"
#include "ExplodingMangoProjectile.generated.h"

class UProjectileMovementComponent;
class UNiagaraComponent;
class UBoxComponent;
class USphereComponent;
class UGameplayEffect;
class UNiagaraSystem;

UCLASS()
class PROLOGUE_API AExplodingMangoProjectile : public APrologueProjectileBase
{
	GENERATED_BODY()

public:
	AExplodingMangoProjectile();

	/* Sejin */
	void SetPoolRef(Pool<AExplodingMangoProjectile>* PoolRef);

	UFUNCTION()
	void Active(FVector Location, FRotator Rotation);

	void Deactivate();
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	virtual void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	void StickAndExplosion(const FHitResult& Hit);
	
	void Explode();

	void OnExplosionEffectFinished(UNiagaraComponent* Niagara);

protected:
	float ElapsedTime = 0.f;

	bool bHasExploded = false;

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
	TObjectPtr<UNiagaraSystem> ProjectileEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Effect")
	TObjectPtr<UNiagaraComponent> ExplosionEffectComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Effect")
	TObjectPtr<UNiagaraComponent> ProjectileEffectComponent;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Effect")
	TObjectPtr<USoundBase> ExplosionSound;

	/* Sejin Section */
	Pool<AExplodingMangoProjectile>* MyPool;
};
