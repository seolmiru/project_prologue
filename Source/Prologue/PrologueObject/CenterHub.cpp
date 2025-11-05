// Fill out your copyright notice in the Description page of Project Settings.


#include "CenterHub.h"

#include "NiagaraComponent.h"
#include "Prologue/Game/PrologueGameInstance.h"

ACenterHub::ACenterHub()
{
	PrimaryActorTick.bCanEverTick = false;

	CenterHubMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CenterHubMesh"));
	RootComponent = CenterHubMesh;

	ActivateNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ActivateNiagara"));
	ActivateNiagara->SetupAttachment(RootComponent);
	ActivateNiagara->SetAutoActivate(false);
}

void ACenterHub::BeginPlay()
{
	Super::BeginPlay();

	if (CenterHubMesh)
	{
		DynamicMaterial = CenterHubMesh->CreateAndSetMaterialInstanceDynamic(0);

		UPrologueGameInstance* GameInstance = Cast<UPrologueGameInstance>(GetGameInstance());
		if (GameInstance)
		{
			GameInstance->RegisterCenterHub(this);
		}
	}
}

void ACenterHub::UpdateAppearance(int32 ActivatedCount)
{
	if (DynamicMaterial && TextureStates.IsValidIndex(ActivatedCount) && TextureStates[ActivatedCount])
	{
		DynamicMaterial->SetTextureParameterValue(TextureParameterName, TextureStates[ActivatedCount]);

		if (ActivatedCount == 3)
		{
			ActivateCenterHubNiagara();
		}
	}
}

void ACenterHub::ActivateCenterHubNiagara()
{
	if (ActivateNiagara)
	{
		ActivateNiagara->Activate();
	}
}

