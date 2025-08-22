// Fill out your copyright notice in the Description page of Project Settings.


#include "BackgroundMusicSubsystem.h"

#include "MetasoundSource.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/Prologue.h"

void UBackgroundMusicSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LOG_SCREEN_R("Subsystem initialize");
		
	MusicAudioComponent = nullptr;
	bIsInCombatZone = false;
}

void UBackgroundMusicSubsystem::Deinitialize()
{
	if (MusicAudioComponent && !MusicAudioComponent->IsBeingDestroyed())
	{
		MusicAudioComponent->Stop();
		MusicAudioComponent->DestroyComponent();
	}

	MusicAudioComponent = nullptr;
	
	Super::Deinitialize();
}

void UBackgroundMusicSubsystem::SetAndPlayMusic(UMetaSoundSource* MusicAsset)
{
	if (!MusicAsset)
	{
		LOG_SCREEN_R("Not Valid music");
		return;
	}

	if (!MusicAudioComponent)
	{
		if (UWorld* World = GetWorld())
		{
			MusicAudioComponent = NewObject<UAudioComponent>(World);

			if (MusicAudioComponent)
			{
				MusicAudioComponent->RegisterComponent();
			}
		}
	}

	if (!MusicAudioComponent)
	{
		LOG_SCREEN_R("MusicAudioComponent not spawn");
		return;
	}

	LOG_SCREEN_R("Valid Music, Play Music");
	MusicAudioComponent->SetSound(MusicAsset);
	MusicAudioComponent->Play();

	UpdateMusicState();
}

void UBackgroundMusicSubsystem::EnterCombatZone()
{
	if (!bIsInCombatZone)
	{
		bIsInCombatZone = true;
		UpdateMusicState();
	}
}

void UBackgroundMusicSubsystem::EnterSafeZone()
{
	if (bIsInCombatZone)
	{
		bIsInCombatZone = false;
		UpdateMusicState();
	}
}

void UBackgroundMusicSubsystem::EnterEliteZone()
{
	if (!bIsEliteZone)
	{
		bIsEliteZone = true;
		UpdateMusicState();
	}
}

void UBackgroundMusicSubsystem::UpdateMusicState()
{
	if (MusicAudioComponent && MusicAudioComponent->IsPlaying())
	{
		MusicAudioComponent->SetBoolParameter(FName("CombatState"), bIsInCombatZone);
		MusicAudioComponent->SetBoolParameter(FName("EliteZone"), bIsEliteZone);
	}
}
