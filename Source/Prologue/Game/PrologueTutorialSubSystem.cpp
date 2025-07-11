// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueTutorialSubSystem.h"

#include "AbilitySystemComponent.h"
#include "AnimNodes/AnimNode_RandomPlayer.h"
#include "Prologue/Character/Player/Comma.h"
#include "Prologue/UI/TutorialWidget.h"

void UPrologueTutorialSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LOG_SCREEN_R("Tutorial SubSystem Initialized");
	LOG_SCREEN_R("Tutorial Data Count : %d", TutorialDataList.Num());
}

void UPrologueTutorialSubSystem::Deinitialize()
{
	UnregisterPlayer();
	Super::Deinitialize();
}

void UPrologueTutorialSubSystem::RegisterPlayer(AComma* Comma)
{
	if (!Comma || !Comma->GetAbilitySystemComponent())
	{
		LOG_SCREEN_R("Register player failed");
		return;
	}

	UnregisterPlayer();
	RegisteredPlayer = Comma;

	UAbilitySystemComponent* ASC = Comma->GetAbilitySystemComponent();

	for (const FTutorialData& Tutorial : TutorialDataList)
	{
		if (Tutorial.TriggerTag.IsValid())
		{
			FDelegateHandle Handle = ASC->RegisterGameplayTagEvent(
				Tutorial.TriggerTag,
				EGameplayTagEventType::NewOrRemoved
			).AddUObject(this, &UPrologueTutorialSubSystem::OnTagChanged);

			TagEventHandles.Add(Tutorial.TriggerTag, Handle);
		}
	}

	LOG_SCREEN_R("Registered %d tutorial tags", TagEventHandles.Num());
}

void UPrologueTutorialSubSystem::UnregisterPlayer()
{
	if (RegisteredPlayer.IsValid() && RegisteredPlayer->GetAbilitySystemComponent())
	{
		UAbilitySystemComponent* ASC = RegisteredPlayer->GetAbilitySystemComponent();

		for (auto& Pair : TagEventHandles)
		{
			ASC->UnregisterGameplayTagEvent(Pair.Value, Pair.Key, EGameplayTagEventType::NewOrRemoved);
		}
	}

	TagEventHandles.Empty();
	RegisteredPlayer.Reset();
}

void UPrologueTutorialSubSystem::OnTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	LOG_SCREEN_R("OnTageChanged : %s, Count : %d", *Tag.ToString(), NewCount);
	
	if (NewCount > 0)
	{
		for (const FTutorialData& Tutorial : TutorialDataList)
		{
			if (Tutorial.TriggerTag == Tag)
			{
				if (Tutorial.bShowOnce && ShownTutorials.Contains(Tag))
					return;

				ShowTutorial(Tutorial);
				ShownTutorials.AddUnique(Tag);
				break;
			}
		}
	}
}

void UPrologueTutorialSubSystem::ShowTutorial(const FTutorialData& TutorialData)
{
	LOG_SCREEN_R("ShowTutorial Called for : %s", *TutorialData.TriggerTag.ToString());
	
	if (!TutorialData.TutorialWidgetClass || !RegisteredPlayer.IsValid())
	{
		return;
	}

	APlayerController* PC = RegisteredPlayer->GetController<APlayerController>();
	if (!PC)
		return;

	PC->SetPause(true);

	CurrentTutorialWidget = CreateWidget<UTutorialWidget>(PC, TutorialData.TutorialWidgetClass);
	if (CurrentTutorialWidget)
	{
		CurrentTutorialWidget->SetTutorialData(TutorialData);
		CurrentTutorialWidget->OnTutorialClosed.AddDynamic(this, &UPrologueTutorialSubSystem::OnTutorialClosed);
		CurrentTutorialWidget->AddToViewport(100);

		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(CurrentTutorialWidget->TakeWidget());
		PC->SetInputMode(InputMode);
	}
}

void UPrologueTutorialSubSystem::HideTutorial()
{
	if (CurrentTutorialWidget)
	{
		CurrentTutorialWidget->RemoveFromParent();
		CurrentTutorialWidget = nullptr;
	}

	if (RegisteredPlayer.IsValid())
	{
		if (APlayerController* PC = RegisteredPlayer->GetController<APlayerController>())
		{
			PC->SetPause(false);

			FInputModeGameAndUI InputMode;
			PC->SetInputMode(InputMode);
		}
	}
}

void UPrologueTutorialSubSystem::OnTutorialClosed()
{
	HideTutorial();
}
