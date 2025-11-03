// Fill out your copyright notice in the Description page of Project Settings.

#include "ChronosProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"

AChronosProjectile::AChronosProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMovement->InitialSpeed = 1500.f;
	ProjectileMovement->MaxSpeed = 1500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	CurrentState = EProjectileState::MovingForward;
}

void AChronosProjectile::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
}

void AChronosProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == EProjectileState::MovingForward)
	{
		const float DistanceTraveled = FVector::Dist(StartLocation, GetActorLocation());

		if (DistanceTraveled >= MaxTravelDistance)
		{
			CurrentState = EProjectileState::Returning;

			ProjectileMovement->Velocity = -ProjectileMovement->Velocity;
		}
	}
	else
	{
		const FVector DirToStart = (StartLocation - GetActorLocation()).GetSafeNormal();
		const float DotProduct = FVector::DotProduct(DirToStart, ProjectileMovement->Velocity.GetSafeNormal());

		if (DotProduct < 0.f)
		{
			Destroy();
		}
	}
}

