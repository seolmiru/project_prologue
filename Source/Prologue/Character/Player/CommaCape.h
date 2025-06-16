// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CommaCape.generated.h"

UCLASS()
class PROLOGUE_API ACommaCape : public AActor
{
	GENERATED_BODY()

public:
	ACommaCape();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cape")
	USkeletalMeshComponent* CapeMesh;
};
