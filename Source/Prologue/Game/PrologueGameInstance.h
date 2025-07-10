// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MoviePlayer.h"
#include "PrologueGameInstance.generated.h"

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
	void SetHasIntroDialoguePlayed(bool bPlayed) { bHasIntroDialoguePlayed = bPlayed; }

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool GetHasIntroDialoguePlayed() const { return bHasIntroDialoguePlayed; }
	
protected:
	void OnPreLoadMap(const FString& MapName);
	void OnDestinationWorldLoaded(UWorld* LoadedWorld);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading Screen")
	TArray<TSoftClassPtr<UUserWidget>> LoadingScreenWidgets;

private:
	TSharedPtr<SWidget> CreateRandomLoadingWidget();
};
