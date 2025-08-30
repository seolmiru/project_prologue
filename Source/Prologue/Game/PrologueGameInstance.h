// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/GameInstance.h"
#include "MoviePlayer.h"
#include "PrologueGameInstance.generated.h"

class UPrologueSaveGame;
class AComma;
/**
 * 
 */

UCLASS()
class PROLOGUE_API UPrologueGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	bool bHasIntroDialoguePlayed = false;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetHasIntroDialoguePlayed(bool bPlayed);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool GetHasIntroDialoguePlayed() const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool HasTriggerPlayed(FName TriggerID);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void MarkTriggerPlayed(FName TriggerID);	

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool HasPowerBankInteracted(FName PowerBankID) const;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void MarkPowerBankInteracted(FName PowerBankID);
	
	UFUNCTION(BlueprintCallable, Category = "Save System")
	bool HasSavedGame() const;

	UFUNCTION(BlueprintCallable, Category = "Save System")
	void SaveGameProgress(const FString& LevelName);

	UFUNCTION(BlueprintCallable, Category = "Save System")
	FString GetSavedLevelName() const;
	
protected:
	void OnPreLoadMap(const FString& MapName);
	void OnDestinationWorldLoaded(UWorld* LoadedWorld);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading Screen")
	TArray<TSoftClassPtr<UUserWidget>> LoadingScreenWidgets;

private:
	TSharedPtr<SWidget> CreateRandomLoadingWidget();

	UPROPERTY()
	UPrologueSaveGame* SaveGameData;

	FString SaveSlotName = "savegame";
	uint32 UserIndex = 0;

	TSet<FName> PlayedTriggerIDs;

	TSet<FName> InteractedPowerBankIDs;
};
