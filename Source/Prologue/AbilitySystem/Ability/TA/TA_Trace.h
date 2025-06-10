// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TA_Trace.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API ATA_Trace : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	ATA_Trace();

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;

protected:
	virtual FGameplayAbilityTargetDataHandle MakeTargetData() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float TraceLength;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float TraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	bool bUseFanShapeTrace = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|FanShape")
	float Angle = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|FanShape")
	int32 NumTraces = 5;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bShowDebug = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ECollisionChannel> TraceChannel;
};
