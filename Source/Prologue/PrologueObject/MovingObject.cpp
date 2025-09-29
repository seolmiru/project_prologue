// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingObject.h"

#include "IMediaControls.h"


AMovingObject::AMovingObject()
{
	PrimaryActorTick.bCanEverTick = true;

	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectMesh"));
	ObjectMesh->SetupAttachment(RootComponent);
}

void AMovingObject::BeginPlay()
{
	Super::BeginPlay();

	InitLocation = GetActorLocation();

	SetActorTickEnabled(false);
}

void AMovingObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsMoving)
	{
		const FVector CurrentLocation = GetActorLocation();

		if (CurrentLocation.Equals(TargetLocation, 1.f))
		{
			bIsMoving = false;

			SetActorTickEnabled(false);

			SetActorLocation(TargetLocation);
		}
		else
		{
			const FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, MovementSpeed);
			SetActorLocation(NewLocation);
		}
	}
}

void AMovingObject::StartMovement()
{
	if (bIsMoving || GetActorLocation().Equals(TargetLocation, 1.f))
	{
		return;
	}

	bIsMoving = true;

	SetActorTickEnabled(true);
}

void AMovingObject::ResetToInitLocation()
{
	bIsMoving = false;
	SetActorTickEnabled(false);
	SetActorLocation(InitLocation);
}

