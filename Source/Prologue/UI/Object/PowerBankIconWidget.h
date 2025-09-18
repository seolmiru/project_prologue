// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PowerBankIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UPowerBankIconWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PowerBank")
	class APowerBank* PowerBank;
};
