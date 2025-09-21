// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrologueGameMode.h"
#include "Prologue/Character/PrologueCharacter.h"
#include "Prologue/Controller/CommaController.h"
#include "Prologue/PlayerState/ProloguePlayerState.h"
#include "Prologue/UI/TutorialGuide.h"
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

void APrologueGameMode::SpawnGuide()
{
	if (TutorialGuideClass && !TutorialGuideActor)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		TutorialGuideActor = GetWorld()->SpawnActor<ATutorialGuide>(TutorialGuideClass, GuideSpawnLocation, FRotator::ZeroRotator, SpawnParams);
	}
}

void APrologueGameMode::BeginPlay()
{
	Super::BeginPlay();

	SpawnGuide();
}
