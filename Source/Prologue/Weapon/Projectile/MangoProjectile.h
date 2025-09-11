// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrologueProjectileBase.h"
#include "MangoProjectile.generated.h"

class UNiagaraSystem;

UCLASS()
class PROLOGUE_API AMangoProjectile : public APrologueProjectileBase
{
	GENERATED_BODY()

public:
	AMangoProjectile();
	
protected:
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	float ExplosionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	float TimeToExplode;

	FTimerHandle ExplosionTimerHandle;

	float ElapsedTime = 0.f;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara")
	TObjectPtr<UNiagaraSystem> ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<USoundBase> ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bShowDebug = true;

protected:
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	void StickAndExplosion(const FHitResult& Hit);

	void Explode();
};