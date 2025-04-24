// Fill out your copyright notice in the Description page of Project Settings.


#include "CommaController.h"

void ACommaController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
