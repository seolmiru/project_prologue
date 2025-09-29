// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrologueUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "Prologue/AbilitySystem/Attribute/PrologueAttributeSet.h"
#include "FloatingDamage.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UFloatingDamage : public UPrologueUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;

	virtual void OnDamageChanged(const FOnAttributeChangeData& ChangeData);

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float Damage = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FText DamageText;

protected:
	void UpdateDamageText();
};
