// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_PlaySound3D.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UAnimNotify_PlaySound3D : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	USoundBase* Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	float VolumeMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	float PitchMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FName AttachName;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;
};
