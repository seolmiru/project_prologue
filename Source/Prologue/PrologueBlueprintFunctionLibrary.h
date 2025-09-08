// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PrologueBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UPrologueBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Effect나 Actor를 플랫폼 높이에 딱 맞춰서 스폰시키기 위한 함수
	UFUNCTION(BlueprintPure, Category = "Prologue|Utility")
	static bool FindGroundLocation(const UObject* WorldContextObject, const FVector& SearchPoint, FVector& OutGroundLocation);
};
