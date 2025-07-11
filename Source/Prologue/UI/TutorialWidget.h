// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prologue/Game/PrologueTutorialSubSystem.h"
#include "TutorialWidget.generated.h"

struct FTutorialData;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTutoralClosed);

/**
 * 
 */
UCLASS()
class PROLOGUE_API UTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetTutorialData(const FTutorialData& InTutorialData);

	UPROPERTY(BlueprintAssignable)
	FOnTutoralClosed OnTutorialClosed;

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TitleText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DescriptionText;

	UPROPERTY(meta = (BindWidget))
	class UImage* VideoDisplay;

	UPROPERTY(meta = (BindWidget))
	class UButton* CloseButton;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Media")
	class UMediaPlayer* MediaPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Media")
	class UMediaTexture* MediaTexture;

	UFUNCTION()
	void OnMediaOpenFailed(FString FailedUrl);

	UFUNCTION()
	void OnMediaOpened(FString OpenedUrl);

	FTutorialData TutorialData;
};
