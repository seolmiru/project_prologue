// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prologue/AbilitySystem/Attribute/PrologueAttributeSet.h"
#include "Prologue/AbilitySystem/Attribute/PrologueSkillAttributeSet.h"
#include "Prologue/UI/PrologueUserWidget.h"
#include "CommaWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UCommaWidget : public UPrologueUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;

	virtual void OnCurrentHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData);

	virtual void OnCurrentGaugeChanged(const FOnAttributeChangeData& ChangeData);
	virtual void OnMaxGaugeChanged(const FOnAttributeChangeData& ChangeData);

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* GaugeImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* RainbowGaugeImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* GaugeMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* RainbowGaugeMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* GaugeMaterialInstance;

	UPROPERTY()
	UMaterialInstanceDynamic* RainbowGaugeMaterialInstance;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float CurrentHealth = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float CurrentMaxHealth = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float CurrentGauge = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float CurrentMaxGauge = 0.0f;

private:
	void UpdateGaugePercent();
};
