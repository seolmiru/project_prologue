// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutorialWidget.generated.h"

class UTextBlock;
class UGameplayAbility;
/**
 * 
 */
UCLASS()
class PROLOGUE_API UTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void OnAbilityActivated(UGameplayAbility* ActivatedAbility);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LeftClickText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RightClickText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ShiftText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SmashText;
};
