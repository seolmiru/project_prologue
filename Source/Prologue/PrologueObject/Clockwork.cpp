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

	const float PitchValue = RotationSpeed * DeltaTime;

	const FRotator DeltaRotation = FRotator(PitchValue, 0.f, 0.f);

	AddActorLocalRotation(DeltaRotation);
}

