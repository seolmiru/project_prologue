// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BazierProjectile.generated.h"

class UNiagaraComponent;
class UBoxComponent;
class UNiagaraSystem;

UCLASS()
class PROLOGUE_API ABazierProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ABazierProjectile();
	//virtual ~ABazierProjectile() override;

	void FireInDirection(const FVector& ShootDirection);

	// 투사체 반사당했을 때 호출
	void Deflected(AActor* DeflectingActor);
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UBoxComponent> ProjectileCollisionBox;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UNiagaraComponent> ProjectileNiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	float ExplosionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	float TimeToExplode;

	bool bIsStuck = false;
	float ElapsedTime = 0.f;
	
	FTimerHandle ExplosionTimerHandle;

protected:
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<class UGameplayEffect> AttackDamageEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowDebug = true;
	
protected:
	UFUNCTION()
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void StickAndExplosion(const FHitResult& Hit);

	void Explode();

	// Bazier Section
protected:
	const float Root3 = 1.732;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bazier")
	float FireSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bazier")
	float BazierWeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bazier")
	float GroundOffset;

	// 베지어 기준점들
	UPROPERTY()
	TArray<FVector> BazierPoints;

	// 베지어 포인트 설정 코드
	void SetBazierPoint(FVector MyLocation, FVector TargetLocation);
	
	// 베지어 위치 산출 코드
	FVector GetBazierPoint(float weight);

	// 베지어 가중치 계산
	FORCEINLINE float GetWeight(){ return CurrentFlyTime / FlyTime; }
	
	bool bFire;
	float FlyTime;
	float CurrentFlyTime;

	// Niagara Section
/*protected:
	UFUNCTION()
	void SyncNiagaraSpeed(float NewTimeScale);	*/


protected:
	// 투사체가 패링 당했을 때의 상태 관리
	UPROPERTY()
	bool bIsDeflected = false;

	UPROPERTY()
	TObjectPtr<AActor> OriginalTarget;
};
