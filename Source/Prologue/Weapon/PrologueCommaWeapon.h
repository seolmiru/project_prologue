// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrologueWeaponBase.h"
#include "Prologue/Types/PrologueStructTypes.h"
#include "PrologueCommaWeapon.generated.h"

UCLASS()
class PROLOGUE_API APrologueCommaWeapon : public APrologueWeaponBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
	FCommaWeaponData CommaWeaponData;
};
