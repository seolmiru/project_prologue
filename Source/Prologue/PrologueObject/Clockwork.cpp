// Fill out your copyright notice in the Description page of Project Settings.


#include "Clockwork.h"

AClockwork::AClockwork()
{
	PrimaryActorTick.bCanEverTick = true;

	ClockworkMesh = CreateDefaultSubobject<UStaticMeshComponent>("ClockworkMesh");
	ClockworkMesh->SetupAttachment(GetRootComponent());
}

void AClockwork::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RotationAxis == ERotationAxis::None)
	{
		return;
	}

	const float AxisValue = RotationSpeed * DeltaTime;

	FRotator DeltaRotation = FRotator::ZeroRotator;

	switch (RotationAxis)
	{
		case ERotationAxis::Pitch:
			DeltaRotation.Pitch = AxisValue;
			break;
		case ERotationAxis::Yaw:
			DeltaRotation.Yaw = AxisValue;
			break;
		case ERotationAxis::Roll:
			DeltaRotation.Roll = AxisValue;
			break;
		default:
			break;
	}
	
	AddActorLocalRotation(DeltaRotation);
}

