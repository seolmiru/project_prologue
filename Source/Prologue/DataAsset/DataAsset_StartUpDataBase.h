// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset_StartUpDataBase.generated.h"

class UGameplayAbility;
class UPrologueAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class PROLOGUE_API UDataAsset_StartUpDataBase : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UPrologueAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf<UGameplayAbility>> ActivateOnGivenAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf<UGameplayAbility>> ReactiveAbilities;

	void GrantAbilities(const TArray<TSubclassOf<UGameplayAbility>> &InAbilitiesToGive, UPrologueAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1);
};
