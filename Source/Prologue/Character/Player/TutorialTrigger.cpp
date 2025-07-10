// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialTrigger.h"

#include "DialogueManager.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/Game/PrologueGameInstance.h"


ATutorialTrigger::ATutorialTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATutorialTrigger::BeginPlay()
{
	Super::BeginPlay();

	UPrologueGameInstance* GameInstance = Cast<UPrologueGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (GameInstance && GameInstance->GetHasIntroDialoguePlayed())
	{
		return;
	}
		
	if (DialogueManagerClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		DialogueManager = GetWorld()->SpawnActor<ADialogueManager>(DialogueManagerClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);

		if (DialogueManager)
		{
			if (GameInstance)
			{
				GameInstance->SetHasIntroDialoguePlayed(true);
			}
			
			GetWorld()->GetTimerManager().SetTimer(StartDialogueTimerHandle, this, &ATutorialTrigger::StartTutorialDialogue, StartDelay, false);
		}
	}
}

void ATutorialTrigger::StartTutorialDialogue()
{
	if (DialogueManager)
	{
		DialogueManager->StartDialogue(StartDialogueID);
	}
}

