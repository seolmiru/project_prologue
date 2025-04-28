// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ComboBowData.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UComboBowData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UComboBowData();

	UPROPERTY(EditAnywhere, Category = "ComboData")
	FString MontageSectionNamePrefix;

	UPROPERTY(EditAnywhere, Category = "ComboData")
	uint8 MaxComboCount;

	UPROPERTY(EditAnywhere, Category = "ComboData")
	float FrameRate;

	UPROPERTY(EditAnywhere, Category = "ComboData")
	TArray<float> EffectiveFrameCount;
};
