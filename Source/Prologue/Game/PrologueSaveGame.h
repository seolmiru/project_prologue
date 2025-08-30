// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PrologueSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UPrologueSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPrologueSaveGame();

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	bool bHasIntroDialoguePlayed = false;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	bool bHasGameProgress = false;
	
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString SavedLevelName = "";

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<FName> PlayedTriggerID;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<FName> InteractedPowerBankID;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	int32 ActivatedPowerBankCount = 0;
};
