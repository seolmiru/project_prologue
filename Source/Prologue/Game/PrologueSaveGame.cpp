// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueSaveGame.h"

UPrologueSaveGame::UPrologueSaveGame()
{
}

void UPrologueSaveGame::ResetToDefault()
{
	bHasIntroDialoguePlayed = false;
	bHasGameProgress = false;
	SavedLevelName = "";
	PlayedTriggerID.Empty();
	InteractedPowerBankID.Empty();
	ActivatedPowerBankCount = 0;

	SavedHealth = 100.f;
	SavedCurrency = 0.f;
}
