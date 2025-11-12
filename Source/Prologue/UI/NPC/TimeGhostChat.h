// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimeGhostChat.generated.h"

class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class PROLOGUE_API UTimeGhostChat : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetChatMessage(const FText& Message);

protected:
	UPROPERTY(BlueprintReadOnly)
	FText ChatMessage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ChatBoxImage;	

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MessageText;
};
