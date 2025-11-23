// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Clockwork.generated.h"

UENUM(BlueprintType)
enum class ERotationAxis : uint8
{
	None UMETA(DisplayName  = "None"),
	Pitch UMETA(DisplayName  = "Pitch"),
	Yaw UMETA(DisplayName  = "Yaw"),
	Roll UMETA(DisplayName  = "Roll")
};

UCLASS()
class PROLOGUE_API AClockwork : public AActor
{
	GENERATED_BODY()

public:
	AClockwork();

	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ClockworkMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RotationSpeed = -30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ERotationAxis RotationAxis = ERotationAxis::Pitch;	
};
