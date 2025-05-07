// Fill out your copyright notice in the Description page of Project Settings.


#include "CommaController.h"

ACommaController::ACommaController()
{
	CommaTeamID = FGenericTeamId(0);
}

FGenericTeamId ACommaController::GetGenericTeamId() const
{
	return CommaTeamID;
}

void ACommaController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
