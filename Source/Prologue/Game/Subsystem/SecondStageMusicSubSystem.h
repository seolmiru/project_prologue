// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SecondStageMusicSubSystem.generated.h"

/**
 * 
 */
UCLASS(Config = Game)
class PROLOGUE_API USecondStageMusicSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Music")
	TObjectPtr<USoundBase> StageBackgroundMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Music")
	TObjectPtr<USoundBase> BossFirstPhaseMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Music")
	TObjectPtr<USoundBase> BossSecondPhaseMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Music")
	float MusicFadeDuration = 2.f;

	UFUNCTION(BlueprintCallable, Category = "Music")
	void HandlePlayerEnterBossArea();

	UFUNCTION(BlueprintCallable, Category = "Music")
	void HandlePlayerLeaveBossArea();

	void HandleBossStartSecondPhase();

	UFUNCTION(BlueprintCallable)
	void InItMusicAssets(USoundBase* InStageMusic, USoundBase* InFirstPhaseMusic, USoundBase* InSecondPhaseMusic, float InFadeDuration = 1.f);
	
protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAudioComponent> StageBackgroundMusicComponent;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAudioComponent> BossFirstPhaseMusicComponent;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAudioComponent> BossSecondPhaseMusicComponent;

private:
	bool bIsInBossArea = false;
	bool bIsBossInSecondPhase = false;

	void PlayerAndPause(UAudioComponent* Comp, USoundBase* Sound);
};
