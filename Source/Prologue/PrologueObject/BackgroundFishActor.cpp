// Fill out your copyright notice in the Description page of Project Settings.

#include "BackgroundFishActor.h"

ABackgroundFishActor::ABackgroundFishActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABackgroundFishActor::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	CurrentAngle = FMath::DegreesToRadians(StartAngleOffset);
}

void ABackgroundFishActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float AngularVelocity = FMath::DegreesToRadians(MovementSpeed);

	CurrentAngle += AngularVelocity * DeltaTime;

	float X = StartLocation.X + OrbitRadius * FMath::Cos(CurrentAngle);
	float Y = StartLocation.Y + OrbitRadius * FMath::Sin(CurrentAngle);
	float Z = StartLocation.Z;

	FVector NewLocation(X, Y, Z);
	SetActorLocation(NewLocation);

	FVector TangentDirection = FVector(-FMath::Sin(CurrentAngle), FMath::Cos(CurrentAngle), 0.0f);
	TangentDirection.Normalize();

	FRotator NewRotation = FRotationMatrix::MakeFromX(TangentDirection).Rotator();
	SetActorRotation(NewRotation);
}