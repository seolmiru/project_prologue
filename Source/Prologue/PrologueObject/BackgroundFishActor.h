// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "BackgroundFishActor.generated.h"

UCLASS()
class PROLOGUE_API ABackgroundFishActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABackgroundFishActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementSpeed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float OrbitRadius = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StartAngleOffset = 0.f;
	
private:
	FVector StartLocation;

	float CurrentAngle;
};