// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PrologueTutorialSubSystem.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FTutorialData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag TriggerTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UTutorialWidget> TutorialWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMediaSource* TutorialVideo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TutorialTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TutorialDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowOnce = true;
};

UCLASS(Blueprintable)
class PROLOGUE_API UPrologueTutorialSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void RegisterPlayer(class AComma* Comma);
	void UnregisterPlayer();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	TArray<FTutorialData> TutorialDataList;
	
protected:
	UPROPERTY()
	TWeakObjectPtr<class AComma> RegisteredPlayer;

	TMap<FGameplayTag, FDelegateHandle> TagEventHandles;

	UPROPERTY()
	TArray<FGameplayTag> ShownTutorials;

	UPROPERTY()
	class UTutorialWidget* CurrentTutorialWidget;
	
	void OnTagChanged(const FGameplayTag Tag, int32 NewCount);
	void ShowTutorial(const FTutorialData& TutorialData);
	void HideTutorial();

	UFUNCTION()
	void OnTutorialClosed();
};

