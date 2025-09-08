// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prologue/AbilitySystem/Attribute/PrologueAttributeSet.h"
#include "Prologue/AbilitySystem/Attribute/PrologueSkillAttributeSet.h"
#include "Prologue/UI/PrologueUserWidget.h"
#include "CommaWidget.generated.h"

class UHorizontalBox;
class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class PROLOGUE_API UCommaWidget : public UPrologueUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;

	virtual void OnCurrentHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData);

	virtual void OnCooldownTagChanged(const FGameplayTag Tag, int32 NewCount);

	virtual void OnCurrentHealPotionChanged(const FOnAttributeChangeData& ChangeData);
	virtual void OnMaxHealPotionChanged(const FOnAttributeChangeData& ChangeData);

	virtual void OnCurrencyChanged(const FOnAttributeChangeData& ChangeData);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> GaugeImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> RainbowGaugeImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PlayerIconOverClock;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> HealPotionContainer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInterface> GaugeMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInterface> RainbowGaugeMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> GaugeMaterialInstance;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> RainbowGaugeMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
	TObjectPtr<UTexture2D> FullHealPotionTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
	TObjectPtr<UTexture2D> EmptyHealPotionTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
	int32 MaxDisplayHealPotions = 5;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
	TArray<TObjectPtr<UImage>> HealPotionImages;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	FGameplayTag CooldownTag;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float CurrentHealth = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float CurrentMaxHealth = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float CurrentGauge = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float CurrentMaxGauge = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float CurrentHealPotion = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float MaxHealPotion;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 Currency = 0;

	virtual void OnOverClockTagChanged(const FGameplayTag Tag, int32 NewCount);

	UFUNCTION(BlueprintCallable)
	void UpdateGaugePercent(float Percent);

	UFUNCTION(BlueprintCallable)
	void UpdateHealPotion(int32 CurrentPotions, int32 MaxPotions);

private:
	void CreateHealPotionImages();
};
