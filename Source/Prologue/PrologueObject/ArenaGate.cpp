// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaGate.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"

AArenaGate::AArenaGate()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	GateEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GateEffect"));
	GateEffect->SetupAttachment(RootComponent);
	
	GateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
	GateMesh->SetupAttachment(RootComponent);
	GateMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GateMesh->SetCollisionResponseToAllChannels(ECR_Block);
	GateMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GateMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GateMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
	GateMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Ignore);
	
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(RootComponent);
	TriggerVolume->SetBoxExtent(FVector(500.f, 500.f, 200.f));
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	TriggerVolume->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
}

void AArenaGate::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoActivate)
	{
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AArenaGate::OnTriggerBeginOverlap);
	}

	if (bIsOpen)
	{
		OpenGate();
	}
	else
	{
		CloseGate();
	}
}

void AArenaGate::OpenGate()
{
	if (!bIsOpen)
	{
		GateEffect->DestroyComponent();

		const FVector GateEffectLocation = GateEffect->GetComponentLocation();

		// Gate 열릴 때 나오는 Niagara 생성
		if (GateDestroyEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				GateDestroyEffect,
				GateEffectLocation,
				FRotator(0, 0, 0),
				FVector(1.f, 1.f, 1.f),
				true,
				true
			);
		}

		AudioComponent->Play();
		
		bIsOpen = true;
		GateMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OnGateStateChanged.Broadcast(true);
	}
}

void AArenaGate::CloseGate()
{
	if (bIsOpen)
	{
		bIsOpen = false;
		GateMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		OnGateStateChanged.Broadcast(false);
	}
}

void AArenaGate::ActivateArena()
{
	CloseGate();

	CheckEnemies();
}

void AArenaGate::CheckEnemies()
{
	for (AActor* Enemy : TrackedEnemies)
	{
		if (IsValid(Enemy))
		{
			Enemy->OnDestroyed.RemoveDynamic(this, &AArenaGate::OnEnemyDestroyed);
		}
	}

	TrackedEnemies.Empty();

	TArray<AActor*> OverlappingActors;
	TriggerVolume->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag(EnemyTag))
		{
			TrackedEnemies.Add(Actor);
			Actor->OnDestroyed.AddDynamic(this, &AArenaGate::OnEnemyDestroyed);
		}
	}

	EnemyCount = TrackedEnemies.Num();

	if (EnemyCount == 0)
	{
		OpenGate();
	}
}

void AArenaGate::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		ActivateArena();

		TriggerVolume->OnComponentBeginOverlap.RemoveDynamic(this, &AArenaGate::OnTriggerBeginOverlap);
	}
}

void AArenaGate::OnEnemyDestroyed(AActor* DestroyedActor)
{
	if (TrackedEnemies.Contains(DestroyedActor))
	{
		TrackedEnemies.Remove(DestroyedActor);
		EnemyCount = TrackedEnemies.Num();

		if (EnemyCount == 0)
		{
			OpenGate();
		}
	}
}

