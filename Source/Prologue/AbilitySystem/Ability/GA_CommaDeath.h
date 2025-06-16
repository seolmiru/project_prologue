// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_CommaDeath.generated.h"

class ACommaController;
class AComma;
/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_CommaDeath : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	AComma* GetCommaCharacterFromActorInfo(); 

	UFUNCTION(BlueprintPure)
	ACommaController* GetCommaControllerFromActorInfo();

private:
	TWeakObjectPtr<AComma> CachedCommaCharacter;
	TWeakObjectPtr<ACommaController> CachedCommaController;
};
