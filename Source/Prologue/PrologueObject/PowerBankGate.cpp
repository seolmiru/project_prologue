// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerBankGate.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/Prologue.h"
#include "Prologue/Game/PrologueGameInstance.h"
#include "Prologue/Game/PrologueSaveGame.h"

APowerBankGate::APowerBankGate()
{
	PrimaryActorTick.bCanEverTick = false;

	GateEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GateEffect"));
	SetRootComponent(GateEffect);
	
	GateCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("GateCollision"));
	GateCollision->SetupAttachment(RootComponent);
	GateCollision->SetBoxExtent(FVector(100.f, 100.f, 10.f));
	GateCollision->SetCollisionResponseToAllChannels(ECR_Block);
}

void APowerBankGate::BeginPlay()
{
	Super::BeginPlay();

	OpenGate();
}

void APowerBankGate::OpenGate()
{
	const FString SaveSlotName = TEXT("savegame");
	UPrologueSaveGame* LoadedSaveData = Cast<UPrologueSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	
	if (LoadedSaveData && LoadedSaveData->ActivatedPowerBankCount >= GateOpenCost)
	{
		GateCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GateEffect->DestroyComponent();
		
		FVector GateEffectLocation = GateEffect->GetComponentLocation();
	
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
	}
}


