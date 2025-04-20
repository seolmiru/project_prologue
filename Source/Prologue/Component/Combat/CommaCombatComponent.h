// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PawnCombatComponent.h"
#include "CommaCombatComponent.generated.h"


class APrologueCommaWeapon;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROLOGUE_API UCommaCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Prologue|Combat")
	APrologueCommaWeapon* GetCommaCarriedWeaponByTag(FGameplayTag InWeaponTag) const;

	UFUNCTION(BlueprintCallable, Category = "Prologue|Combat")
	APrologueCommaWeapon* GetCommaCurrentEquippedWeapon() const;
};
