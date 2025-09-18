// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrologueProjectileBase.h"
#include "GameFramework/Actor.h"
#include "EggBallProjectile.generated.h"

class UGameplayEffect;
class UProjectileMovementComponent;
class UBoxComponent;
class UNiagaraComponent;

UCLASS()
class PROLOGUE_API AEggBallProjectile : public APrologueProjectileBase
{
	GENERATED_BODY()

public:
	AEggBallProjectile();

	void FireInDirection(const FVector& ShootDirection) const;
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	virtual void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};