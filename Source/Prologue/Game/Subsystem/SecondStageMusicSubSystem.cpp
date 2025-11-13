// Fill out your copyright notice in the Description page of Project Settings.


#include "SecondStageMusicSubSystem.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

void USecondStageMusicSubSystem::HandlePlayerEnterBossArea()
{
	if (bIsInBossArea) return;

	bIsInBossArea = true;

	if (StageBackgroundMusicComponent)
	{
		StageBackgroundMusicComponent->FadeOut(MusicFadeDuration, 0.f);
	}

	if (bIsBossInSecondPhase)
	{
		if (BossSecondPhaseMusicComponent)
		{
			BossSecondPhaseMusicComponent->SetPaused(false);
			BossSecondPhaseMusicComponent->FadeIn(MusicFadeDuration, 1.f);
		}
	}
	else
	{
		if (BossFirstPhaseMusicComponent)
		{
			BossFirstPhaseMusicComponent->SetPaused(false);
			BossFirstPhaseMusicComponent->FadeIn(MusicFadeDuration, 1.f);
		}
	}
}

void USecondStageMusicSubSystem::HandlePlayerLeaveBossArea()
{
	if (!bIsInBossArea) return;

	bIsInBossArea = false;

	if (StageBackgroundMusicComponent)
	{
		StageBackgroundMusicComponent->FadeIn(MusicFadeDuration, 1.f);
	}

	if (BossFirstPhaseMusicComponent)
	{
		BossFirstPhaseMusicComponent->FadeOut(MusicFadeDuration, 0.f);
	}

	if (BossSecondPhaseMusicComponent)
	{
		BossSecondPhaseMusicComponent->FadeOut(MusicFadeDuration, 0.f);
	}
}

void USecondStageMusicSubSystem::HandleBossStartSecondPhase()
{
	if (bIsBossInSecondPhase) return;

	bIsBossInSecondPhase = true;

	if (bIsInBossArea)
	{
		if (BossFirstPhaseMusicComponent)
		{
			BossFirstPhaseMusicComponent->FadeOut(MusicFadeDuration, 0.f);
		}

		if (BossSecondPhaseMusicComponent)
		{
			BossSecondPhaseMusicComponent->SetPaused(false);
			BossSecondPhaseMusicComponent->FadeIn(MusicFadeDuration, 1.f);
		}
	}
}

void USecondStageMusicSubSystem::InItMusicAssets(USoundBase* InStageMusic, USoundBase* InFirstPhaseMusic,
	USoundBase* InSecondPhaseMusic, float InFadeDuration)
{
	StageBackgroundMusic = InStageMusic;
	BossFirstPhaseMusic = InFirstPhaseMusic;
	BossSecondPhaseMusic = InSecondPhaseMusic;
	MusicFadeDuration = InFadeDuration;

	UWorld* World = GetWorld();

	if (StageBackgroundMusic)
	{
		StageBackgroundMusicComponent = UGameplayStatics::CreateSound2D(World, StageBackgroundMusic);

		if (StageBackgroundMusicComponent)
		{
			StageBackgroundMusicComponent->Play();
		}
	}

	if (BossFirstPhaseMusic)
	{
		BossFirstPhaseMusicComponent = UGameplayStatics::CreateSound2D(World, BossFirstPhaseMusic);

		if (BossFirstPhaseMusicComponent)
		{
			BossFirstPhaseMusicComponent->Play();
			BossFirstPhaseMusicComponent->SetPaused(true);
		}
	}

	if (BossSecondPhaseMusic)
	{
		BossSecondPhaseMusicComponent = UGameplayStatics::CreateSound2D(World, BossSecondPhaseMusic);

		if (BossSecondPhaseMusicComponent)
		{
			BossSecondPhaseMusicComponent->Play();
			BossSecondPhaseMusicComponent->SetPaused(true);
		}
	}
}

void USecondStageMusicSubSystem::PlayerAndPause(UAudioComponent* Comp, USoundBase* Sound)
{
	if (Sound)
	{
		Comp = UGameplayStatics::CreateSound2D(GetWorld(), Sound);

		if (Comp)
		{
			Comp->Play();
			Comp->SetPaused(true);
		}
	}
}
