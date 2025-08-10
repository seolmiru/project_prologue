// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BackgroundMusicSubsystem.generated.h"

class UMetaSoundSource;
/**
 * 
 */
UCLASS()
class PROLOGUE_API UBackgroundMusicSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Music")
	void SetAndPlayMusic(UMetaSoundSource* MusicAsset);

	UFUNCTION(BlueprintCallable, Category = "Music")
	void EnterCombatZone();

	UFUNCTION(BlueprintCallable, Category = "Music")
	void EnterSafeZone();

private:
	void UpdateMusicState();

	UPROPERTY()
	TObjectPtr<UAudioComponent> MusicAudioComponent;

	bool bIsInCombatZone = false;
};
