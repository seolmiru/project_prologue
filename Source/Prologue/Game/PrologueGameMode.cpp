// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrologueGameMode.h"
#include "Prologue/Character/PrologueCharacter.h"
#include "UObject/ConstructorHelpers.h"

APrologueGameMode::APrologueGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
