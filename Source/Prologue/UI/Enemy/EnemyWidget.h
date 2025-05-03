// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prologue/AbilitySystem/PrologueAttributeSet.h"
#include "Prologue/UI/PrologueUserWidget.h"
#include "EnemyWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UEnemyWidget : public UPrologueUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;

protected:
	virtual void OnHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData);

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float CurrentHealth = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float CurrentMaxHealth = 0.1f;	
};
