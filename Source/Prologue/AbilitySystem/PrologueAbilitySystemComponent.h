// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Prologue/Types/PrologueStructTypes.h"
#include "PrologueAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UPrologueAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void OnAbilityInputPressed(const FGameplayTag& InInputTag);
	void OnAbilityInputReleased(const FGameplayTag& InInputTag);

	UFUNCTION(BlueprintCallable, Category = "Prologue|Ability", meta = (ApplyLevel = "1"))
	void GrantCommaWeaponAbilities(const TArray<FPrologueCommaAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel);
};
