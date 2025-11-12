// Fill out your copyright notice in the Description page of Project Settings.


#include "BossClock_Phase2.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"

ABossClock_Phase2::ABossClock_Phase2()
{
	PrimaryActorTick.bCanEverTick = true;

	MinuteHandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("MinuteHandCollision"));
	MinuteHandCollision->SetupAttachment(ClockHandRoot);
	MinuteHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MinuteHandCollision->SetCollisionResponseToAllChannels(ECR_Ignore);

	MinuteHandeNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MinuteHandeNiagaraComponent"));
	MinuteHandeNiagaraComponent->SetupAttachment(MinuteHandCollision);
}

void ABossClock_Phase2::BeginPlay()
{
	Super::BeginPlay();

	MinuteHandCollision->OnComponentBeginOverlap.AddDynamic(this, &ABossClock::OnBoxOverlap);
}

void ABossClock_Phase2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float YawValue = MinuteHandSpeed * DeltaTime;

	const FRotator DeltaRotation = FRotator(0.f, YawValue, 0.f);

	AddActorLocalRotation(DeltaRotation);
}

