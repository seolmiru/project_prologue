// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerBank.h"

#include "Components/BoxComponent.h"

APowerBank::APowerBank()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(RootComponent);

	PowerBankMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PowerBankMesh"));
	PowerBankMesh->SetupAttachment(RootComponent);

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APowerBank::OnOverlapBegin);
}

void APowerBank::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

