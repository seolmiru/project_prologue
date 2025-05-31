// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MoviePlayer.h"
#include "PrologueGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UPrologueGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

protected:
	void OnPreLoadMap(const FString& MapName);
	void OnDestinationWorldLoaded(UWorld* LoadedWorld);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading Screen")
	TArray<TSoftClassPtr<UUserWidget>> LoadingScreenWidgets;

private:
	TSharedPtr<SWidget> CreateRandomLoadingWidget();
};
