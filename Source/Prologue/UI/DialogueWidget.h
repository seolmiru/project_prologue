// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prologue/DataAsset/DialogueData.h"
#include "DialogueWidget.generated.h"

class UImage;
class UTextBlock;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueCompleted);

/**
 * 
 */
UCLASS()
class PROLOGUE_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogue(FName StartDialogueID);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void NextDialogue();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void EndDialogue();

	void SetCurrentDialogue(const FDialogueData& DialogueData);

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FOnDialogueCompleted OnDialogueCompleted;

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* HourHand;

	UPROPERTY(meta = (BindWidget))
	UImage* MinuteHand;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SpeakerNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DialogueText;

	UPROPERTY(meta = (BindWidget))
	UImage* DialogueBackground;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ContinueText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	UDataTable* DialogueDataTable;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	FName CurrentDialogueID;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	bool bIsDialogueActive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	float TypewriterSpeed = 0.05f;
	
	FTimerHandle TypewriterTimerHandle;
	FString FullDialogueText;
	int32 CurrentCharIndex;

	UFUNCTION()
	void TypewriterEffect();

	void CompleteTypewriter();

private:
	void UpdateCharacterIconStates(const FString& SpeakerName);
};
