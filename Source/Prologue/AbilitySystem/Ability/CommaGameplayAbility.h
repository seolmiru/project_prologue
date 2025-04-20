// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrologueGameplayAbility.h"
#include "CommaGameplayAbility.generated.h"

class UCommaCombatComponent;
class ACommaController;
class AComma;
/**
 * 
 */
UCLASS()
class PROLOGUE_API UCommaGameplayAbility : public UPrologueGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Prologue|Ability")
	AComma* GetCommaFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Prologue|Ability")
	ACommaController* GetCommaControllerFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Prologue|Ability")
	UCommaCombatComponent* GetCombatComponentFromActorInfo();
	
private:
	TWeakObjectPtr<AComma> CachedCommaCharacter;
	TWeakObjectPtr<ACommaController> CachedCommaController;
};
