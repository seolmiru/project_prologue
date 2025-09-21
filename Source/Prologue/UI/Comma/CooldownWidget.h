// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Prologue/UI/PrologueUserWidget.h"
#include "CooldownWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class PROLOGUE_API UCooldownWidget : public UPrologueUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void SetAbilitySystemComponent(AActor* InOwner) override;
	
	virtual void OnCooldownTagChanged(const FGameplayTag Tag, int32 NewCount);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CooldownGaugeImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInterface> CooldownGaugeMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CooldownGaugeMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	FGameplayTag CooldownTag;

	UFUNCTION(BlueprintCallable)
	void UpdateCooldownGaugePercent(float Percent);
};
