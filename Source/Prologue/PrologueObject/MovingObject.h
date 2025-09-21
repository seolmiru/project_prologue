// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingObject.generated.h"

UCLASS()
class PROLOGUE_API AMovingObject : public AActor
{
	GENERATED_BODY()

public:
	AMovingObject();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartMovement();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ResetToInitLocation();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ObjectMesh;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementSpeed = 500.f;

private:
	FVector InitLocation;

	bool bIsMoving = false;
};
