// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossClock.generated.h"

class UGameplayEffect;
class UBoxComponent;
class UNiagaraComponent;

UCLASS()
class PROLOGUE_API ABossClock : public AActor
{
	GENERATED_BODY()

public:
	ABossClock();
	
protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clock")
	TObjectPtr<USceneComponent> ClockHandRoot;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clock")
	TObjectPtr<UNiagaraComponent> ClockHandNiagaraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clock")
	TObjectPtr<UBoxComponent> ClockCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clock|GAS")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clock|GAS")
	TSubclassOf<UGameplayEffect> ImmunityEffectClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clock|Speed")
	float ClockSpeed = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clock|Time")
	float DamageInterval = 1.f;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
