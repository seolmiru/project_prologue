// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CenterHub.generated.h"

class UNiagaraComponent;

UCLASS()
class PROLOGUE_API ACenterHub : public AActor
{
	GENERATED_BODY()

public:
	ACenterHub();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CenterHubMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> ActivateNiagara;
	
	UPROPERTY(EditAnywhere, Category = "Appearance")
	TArray<TObjectPtr<UTexture>> TextureStates;

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FName TextureParameterName;

private:
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

public:
	void UpdateAppearance(int32 ActivatedCount);

	void ActivateCenterHubNiagara();
};
