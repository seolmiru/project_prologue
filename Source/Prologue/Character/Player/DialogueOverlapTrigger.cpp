// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueOverlapTrigger.h"

#include "Comma.h"
#include "DialogueManager.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/Game/PrologueGameInstance.h"

ADialogueOverlapTrigger::ADialogueOverlapTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	RootComponent = TriggerVolume;
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ADialogueOverlapTrigger::OnOverlapBegin);
}

void ADialogueOverlapTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AComma>(OtherActor))
	{
		UPrologueGameInstance* GameInstance = Cast<UPrologueGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (GameInstance)
		{
			if (!GameInstance->HasTriggerPlayed(TriggerID))
			{
				GameInstance->MarkTriggerPlayed(TriggerID);

				ADialogueManager* DialogueManager = Cast<ADialogueManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ADialogueManager::StaticClass()));
				if (DialogueManager)
				{
					DialogueManager->StartDialogue(StartDialogueID);
				}
			}
		}
	}

	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

