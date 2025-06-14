// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EnableAbilityCancel.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UAnimNotify_EnableAbilityCancel : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_EnableAbilityCancel();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
