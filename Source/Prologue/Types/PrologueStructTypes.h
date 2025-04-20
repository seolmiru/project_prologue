// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PrologueStructTypes.generated.h"

class UCommaAnimLayer;

USTRUCT(BlueprintType)
struct FCommaWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UCommaAnimLayer> WeaponAnimLayerToLink;
};