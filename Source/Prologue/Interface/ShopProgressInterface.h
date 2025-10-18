// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ShopProgressInterface.generated.h"

UINTERFACE()
class UShopProgressInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROLOGUE_API IShopProgressInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void UpdatePurchaseProgress(float Progress);
};
