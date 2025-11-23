// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrologueGameMode.h"
#include "Prologue/Character/PrologueCharacter.h"
#include "Prologue/Controller/CommaController.h"
#include "Prologue/PlayerState/ProloguePlayerState.h"
#include "UObject/ConstructorHelpers.h"

APrologueGameMode::APrologueGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Characters/Comma/BP_Comma"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PlayerStateClass = AProloguePlayerState::StaticClass();
	PlayerControllerClass = ACommaController::StaticClass();
}

void APrologueGameMode::BeginPlay()
{
	Super::BeginPlay();
}
