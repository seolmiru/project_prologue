// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PrologueAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROLOGUE_API UPrologueAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ResetCooldownWithMatchingOwningTag(const FGameplayTagContainer CooldownTagContainer);

	UFUNCTION(BlueprintCallable)
	void ReduceCooldownWithMatchingOwningTag(const FGameplayTagContainer CooldownTagContainer, const float ReductionAmount);	
};
