// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttackTutorialWidget.generated.h"

class UGameplayAbility;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class PROLOGUE_API UAttackTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;

	virtual void OnAbilityActivated(UGameplayAbility* ActivatedAbility);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> KeyIconImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TutorialText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> WatchIconImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CheckIconImage;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Time")
	float CustomTimeDilation = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsCompleted = false;

private:
	float TimeDilation = 1.f;
};
