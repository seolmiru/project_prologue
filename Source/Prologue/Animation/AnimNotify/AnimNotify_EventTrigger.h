// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EventTrigger.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UAnimNotify_EventTrigger : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_EventTrigger();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Event")
	FGameplayTag TriggerGameplayTag;
};
