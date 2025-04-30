// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TA_Trace.h"
#include "TA_MultiTrace.generated.h"

UCLASS()
class PROLOGUE_API ATA_MultiTrace : public ATA_Trace
{
	GENERATED_BODY()

protected:
	virtual FGameplayAbilityTargetDataHandle MakeTargetData() const override;
};
