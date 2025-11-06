// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrologueProjectileBase.h"
#include "ChronosScytheProjectile.generated.h"

UENUM(BlueprintType)
enum class EProjectileState : uint8
{
	MovingForward,
	Returning
};

UCLASS()
class PROLOGUE_API AChronosScytheProjectile : public APrologueProjectileBase
{
	GENERATED_BODY()

public:
	AChronosScytheProjectile();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MaxTravelDistance = 2000.f;
	
public:
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	virtual void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
private:
	FVector StartLocation;

	EProjectileState CurrentState;
};
