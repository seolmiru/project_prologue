// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructiblePot.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "Prologue/Character/Player/Comma.h"

ADestructiblePot::ADestructiblePot()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = CollisionComponent;
	CollisionComponent->SetSphereRadius(50.f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionObjectType(ECC_WorldStatic);
	CollisionComponent->SetCollisionResponseToChannels(ECR_Block);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADestructiblePot::BeginPlay()
{
	Super::BeginPlay();
	CollisionComponent->OnComponentHit.AddDynamic(this, &ADestructiblePot::OnHit);
}

void ADestructiblePot::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->IsA<AComma>() && !bDestroyed)
	{
		DestroyPot();
	}
}

void ADestructiblePot::DestroyPot()
{
	if (bDestroyed) return;
	bDestroyed = true;

	if (DestructionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DestructionEffect, GetActorLocation());
	}

	Destroy();
}

