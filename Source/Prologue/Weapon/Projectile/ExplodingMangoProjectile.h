// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrologueProjectileBase.h"
#include "GameFramework/Actor.h"
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

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	virtual void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void StickAndExplosion(const FHitResult& Hit);
	
	void Explode();

protected:
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
};
