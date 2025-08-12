// Fill out your copyright notice in the Description page of Project Settings.


#include "BackgroundMusicManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

ABackgroundMusicManager* ABackgroundMusicManager::Instance = nullptr;

ABackgroundMusicManager::ABackgroundMusicManager()
{
	PrimaryActorTick.bCanEverTick = false;

	MusicAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicAudioComponent"));
	RootComponent = MusicAudioComponent;

	bIsInCombatZone = false;

	Instance = this;
}

void ABackgroundMusicManager::BeginPlay()
{
	Super::BeginPlay();

	if (DynamicMusicAsset)
	{
		StartMusic();
	}
}

void ABackgroundMusicManager::StartMusic()
{
	if (!MusicAudioComponent || !DynamicMusicAsset)
	{
		return;
	}

	MusicAudioComponent->SetSound(DynamicMusicAsset);
	MusicAudioComponent->Play();

	UpdateMusicState();
}

void ABackgroundMusicManager::StopMusic()
{
	if (MusicAudioComponent)
	{
		MusicAudioComponent->Stop();
	}
}

void ABackgroundMusicManager::EnterCombatZone()
{
	if (!bIsInCombatZone)
	{
		bIsInCombatZone = true;
		UpdateMusicState();
	}
}

void ABackgroundMusicManager::EnterSafeZone()
{
	if (bIsInCombatZone)
	{
		bIsInCombatZone = false;
		UpdateMusicState();
	}
}

ABackgroundMusicManager* ABackgroundMusicManager::GetMusicManger()
{
	if (!Instance)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(GEngine->GameViewport, EGetWorldErrorMode::LogAndReturnNull))
		{
			Instance = Cast<ABackgroundMusicManager>(UGameplayStatics::GetActorOfClass(World, ABackgroundMusicManager::StaticClass()));
		}
	}

	return Instance;
}

void ABackgroundMusicManager::UpdateMusicState()
{
	if (MusicAudioComponent && MusicAudioComponent->IsPlaying())
	{
		MusicAudioComponent->SetBoolParameter(FName("CombatState"), bIsInCombatZone);
	}
}

