// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerBankGate.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class UBoxComponent;
class UPrologueSaveGame;

UCLASS()
class PROLOGUE_API APowerBankGate : public AActor
{
	GENERATED_BODY()

public:
	APowerBankGate();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> GateEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> GateCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate|Effect")
	TObjectPtr<UNiagaraSystem> GateDestroyEffect;

protected:
	UFUNCTION(BlueprintCallable)
	void OpenGate();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gate")
	int32 GateOpenCost = 0;
};
