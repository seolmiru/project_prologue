// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MetasoundSource.h"
#include "BackgroundMusicManager.generated.h"

UCLASS()
class PROLOGUE_API ABackgroundMusicManager : public AActor
{
	GENERATED_BODY()

public:
	ABackgroundMusicManager();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* MusicAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	UMetaSoundSource* DynamicMusicAsset;

	UPROPERTY(BlueprintReadOnly, Category = "Music")
	bool bIsInCombatZone;

public:
	UFUNCTION(BlueprintCallable, Category = "Music")
	void StartMusic();

	UFUNCTION(BlueprintCallable, Category = "Music")
	void StopMusic();

	UFUNCTION(BlueprintCallable, Category = "Music")
	void EnterCombatZone();

	UFUNCTION(BlueprintCallable, Category = "Music")
	void EnterSafeZone();

	UFUNCTION(BlueprintCallable, Category = "Music")
	bool IsInCombatZone() const { return bIsInCombatZone; }

	UFUNCTION(BlueprintPure, Category = "Music", meta = (CallInEditor = "true"))
	static ABackgroundMusicManager* GetMusicManger();

private:
	void UpdateMusicState();

	static ABackgroundMusicManager* Instance;
};
