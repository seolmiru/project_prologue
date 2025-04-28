// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ComboAttackData.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UComboAttackData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UComboAttackData();

	UPROPERTY(EditAnywhere, Category = "ComboData")
	FString MontageSectionNamePrefix;

	UPROPERTY(EditAnywhere, Category = "ComboData")
	uint8 MaxComboCount;

	UPROPERTY(EditAnywhere, Category = "ComboData")
	float FrameRate;

	UPROPERTY(EditAnywhere, Category = "ComboData")
	TArray<float> EffectiveFrameCount;
};
