// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAsset_StartUpDataBase.h"
#include "GameplayTagContainer.h"
#include "Prologue/Types/PrologueStructTypes.h"
#include "DataAsset_CommaStartUpData.generated.h"

class UPrologueGameplayAbility;

/**
 * 
 */
UCLASS()
class PROLOGUE_API UDataAsset_CommaStartUpData : public UDataAsset_StartUpDataBase
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UPrologueAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData", meta = (TitleProperty = "InputTag"))
	TArray<FPrologueCommaAbilitySet> CommaStartUpAbilitySets;
};
