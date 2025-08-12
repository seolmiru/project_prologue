// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"
#include "DestructiblePot.generated.h"

UCLASS()
class PROLOGUE_API ADestructiblePot : public AActor
{
	GENERATED_BODY()

public:
	ADestructiblePot();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* CollisionComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraSystem* DestructionEffect;

	bool bDestroyed = false;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	UFUNCTION(BlueprintCallable)
	void DestroyPot();
};
