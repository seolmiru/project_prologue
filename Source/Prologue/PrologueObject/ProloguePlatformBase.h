// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProloguePlatformBase.generated.h"

UCLASS()
class PROLOGUE_API AProloguePlatformBase : public AActor
{
	GENERATED_BODY()

public:
	AProloguePlatformBase();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PlatformMesh;
};
