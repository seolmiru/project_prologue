// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "CommaController.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API ACommaController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ACommaController();

	virtual FGenericTeamId GetGenericTeamId() const override;

protected:
	virtual void BeginPlay() override;

private:
	FGenericTeamId CommaTeamID;
};
