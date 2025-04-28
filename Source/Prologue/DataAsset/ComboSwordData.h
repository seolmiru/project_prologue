// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ComboSwordData.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UComboSwordData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UComboSwordData();

	UPROPERTY(EditAnywhere, Category = "ComboData")
	FString MontageSectionNamePrefix;

	UPROPERTY(EditAnywhere, Category = "ComboData")
	uint8 MaxComboCount;

	UPROPERTY(EditAnywhere, Category = "ComboData")
	float FrameRate;

	UPROPERTY(EditAnywhere, Category = "ComboData")
	TArray<float> EffectiveFrameCount;
};
