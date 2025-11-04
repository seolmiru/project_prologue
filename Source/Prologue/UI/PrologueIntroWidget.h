// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PrologueIntroWidget.generated.h"

class UMediaSoundComponent;
class UMediaPlayer;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FonIntroFinished);

/**
 * 
 */
UCLASS()
class PROLOGUE_API UPrologueIntroWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Intro")
	FonIntroFinished OnIntroFinished;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Intro")
	void PlayIntroAnimation();

	virtual void PlayIntroAnimation_Implementation();

protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION()
	void HandleAnimationFinished();

	UFUNCTION()
	void SkipIntro();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Media")
	TObjectPtr<UMediaPlayer> IntroMediaPlayer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Media")
	TObjectPtr<UMediaSoundComponent> IntroSoundComponent;	

private:
	bool bHasFinished = false;
};
