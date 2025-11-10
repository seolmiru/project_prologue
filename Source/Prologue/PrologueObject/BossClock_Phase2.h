// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossClock.h"
#include "BossClock_Phase2.generated.h"

UCLASS()
class PROLOGUE_API ABossClock_Phase2 : public ABossClock
{
	GENERATED_BODY()

public:
	ABossClock_Phase2();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clock")
	TObjectPtr<UNiagaraComponent> MinuteHandeNiagaraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clock")
	TObjectPtr<UBoxComponent> MinuteHandCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clock")
	float MinuteHandSpeed = 8.f;
};
