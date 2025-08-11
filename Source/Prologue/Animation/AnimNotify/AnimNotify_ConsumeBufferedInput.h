// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ConsumeBufferedInput.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UAnimNotify_ConsumeBufferedInput : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, Category = "Input Buffer")
	bool bConsumeSpecificTag = false;

	UPROPERTY(EditAnywhere, Category = "Input Buffer", meta = (EditCondition = "bConsumeSpecificTag"))
	FGameplayTagContainer SpecificInputTag;

	UPROPERTY(EditAnywhere, Category = "Input Buffer", meta = (EditCondition = "bConsumeSpecificTag"))
	bool bConsumeFirst = true;
};
