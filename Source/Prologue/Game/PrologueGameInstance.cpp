// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueGameInstance.h"

#include "PrologueSaveGame.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/Prologue.h"
#include "Prologue/Character/Player/DialogueManager.h"
#include "Prologue/PrologueObject/CenterHub.h"
#include "Prologue/UI/PrologueIntroWidget.h"

void UPrologueGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ThisClass::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnDestinationWorldLoaded);

	if (LoadingScreenWidgets.Num() == 0)
	{
		LoadingScreenWidgets.Add(TSoftClassPtr<UUserWidget>(FSoftObjectPath(TEXT("/Game/UI/Widget/WBP_LoadingScreen_First.WBP_LoadingScreen_First_C"))));
		LoadingScreenWidgets.Add(TSoftClassPtr<UUserWidget>(FSoftObjectPath(TEXT("/Game/UI/Widget/WBP_LoadingScreen_Second.WBP_LoadingScreen_Second_C"))));
	}

	if (IntroAnimationWidgetClass.IsNull())
	{
		IntroAnimationWidgetClass = TSoftClassPtr<UPrologueIntroWidget>(FSoftObjectPath(TEXT("/Game/UI/Widget/MainMenu/WBP_IntroAnimation.WBP_IntroAnimation_C")));
	}
	
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
	{
		SaveGameData = Cast<UPrologueSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
	}
	else
	{
		SaveGameData = Cast<UPrologueSaveGame>(UGameplayStatics::CreateSaveGameObject(UPrologueSaveGame::StaticClass()));
	}

	if (SaveGameData)
	{
		PlayedTriggerIDs = TSet<FName>(SaveGameData->PlayedTriggerID);
		InteractedPowerBankIDs = TSet<FName>(SaveGameData->InteractedPowerBankID);
		DestroyedAI_IDs = TSet<FName>(SaveGameData->DestroyedAI_IDs);
	}
}

void UPrologueGameInstance::SetHasIntroDialoguePlayed(bool bPlayed)
{
	if (SaveGameData)
	{
		bHasIntroDialoguePlayed = bPlayed;
		SaveGameData->bHasIntroDialoguePlayed = bPlayed;
		UGameplayStatics::SaveGameToSlot(SaveGameData, SaveSlotName, UserIndex);
	}
}

bool UPrologueGameInstance::GetHasIntroDialoguePlayed() const
{
	return SaveGameData ? SaveGameData->bHasIntroDialoguePlayed : false;
}

bool UPrologueGameInstance::HasTriggerPlayed(FName TriggerID)
{
	return PlayedTriggerIDs.Contains(TriggerID);
}

void UPrologueGameInstance::MarkTriggerPlayed(FName TriggerID)
{
	if (!SaveGameData || HasTriggerPlayed(TriggerID))
	{
		return; 
	}

	PlayedTriggerIDs.Add(TriggerID);

	SaveGameData->PlayedTriggerID.Add(TriggerID);

	UGameplayStatics::SaveGameToSlot(SaveGameData, SaveSlotName, UserIndex);
}

bool UPrologueGameInstance::HasPowerBankInteracted(FName PowerBankID) const
{
	return InteractedPowerBankIDs.Contains(PowerBankID);
}

void UPrologueGameInstance::MarkPowerBankInteracted(FName PowerBankID)
{
	if (!SaveGameData || HasPowerBankInteracted(PowerBankID))
	{
		return;
	}

	InteractedPowerBankIDs.Add(PowerBankID);
	SaveGameData->InteractedPowerBankID.Add(PowerBankID);

	UGameplayStatics::SaveGameToSlot(SaveGameData, SaveSlotName, UserIndex);
}

bool UPrologueGameInstance::HasSavedGame() const
{
	return SaveGameData ? SaveGameData->bHasGameProgress : false;
}

void UPrologueGameInstance::SaveGameProgress(const FString& LevelName)
{
	if (SaveGameData)
	{
		SaveGameData->bHasGameProgress = true;
		SaveGameData->SavedLevelName = LevelName;
		UGameplayStatics::SaveGameToSlot(SaveGameData, SaveSlotName, UserIndex);
	}
}

FString UPrologueGameInstance::GetSavedLevelName() const
{
	return SaveGameData ? SaveGameData->SavedLevelName : FString();
}

void UPrologueGameInstance::StartNewGame(const FString& LevelName)
{
	if (!SaveGameData)
	{
		SaveGameData = Cast<UPrologueSaveGame>(UGameplayStatics::CreateSaveGameObject(UPrologueSaveGame::StaticClass()));
		if (!SaveGameData)
		{
			LOG_SCREEN_R("세이브 객체 생성 불가");
			return;
		}
	}

	SaveGameData->ResetToDefault();

	PlayedTriggerIDs.Empty();
	InteractedPowerBankIDs.Empty();
	DestroyedAI_IDs.Empty();
	
	bHasIntroDialoguePlayed = false;
	
	LevelToLoad = LevelName;

	const FString FirstStageLevelName = TEXT("1Stage_VillageofTimekeepers");

	if (!HasSeenInitialIntro() && LevelName.Contains(FirstStageLevelName))
	{
		UClass* WidgetClass = IntroAnimationWidgetClass.LoadSynchronous();
		if (WidgetClass)
		{
			UPrologueIntroWidget* AnimWidget = CreateWidget<UPrologueIntroWidget>(this, WidgetClass);
			if (AnimWidget)
			{
				AnimWidget->OnIntroFinished.AddDynamic(this, &UPrologueGameInstance::OnIntroAnimationFinished);

				AnimWidget->AddToViewport(100);

				AnimWidget->PlayIntroAnimation();
			}
		}
		else
		{
			LOG_SCREEN_R("Failed Load Intro Animation");
			OpenStage();
		}
	}
	else
	{
		OpenStage();
	}
}

void UPrologueGameInstance::OnPowerBankActivated(FName PowerBankID)
{
	if (!SaveGameData || InteractedPowerBankIDs.Contains(PowerBankID))
	{
		return;
	}

	InteractedPowerBankIDs.Add(PowerBankID);
	SaveGameData->InteractedPowerBankID.Add(PowerBankID);
	SaveGameData->ActivatedPowerBankCount++;

	if (WorldCenterHub)
	{
		WorldCenterHub->UpdateAppearance(SaveGameData->ActivatedPowerBankCount);
	}
	else
	{
		LOG_SCREEN_R("CenterHub is Null");
	}
	
	UGameplayStatics::SaveGameToSlot(SaveGameData, SaveSlotName, UserIndex);
}

void UPrologueGameInstance::RegisterCenterHub(ACenterHub* Hub)
{
	WorldCenterHub = Hub;

	if (WorldCenterHub && SaveGameData)
	{
		WorldCenterHub->UpdateAppearance(SaveGameData->ActivatedPowerBankCount);
	}
}

bool UPrologueGameInstance::HasSeenInitialIntro() const
{
	return SaveGameData ? SaveGameData->bHasSeenInitialIntro : false;
}

void UPrologueGameInstance::MarkInitialIntroSeen()
{
	if (SaveGameData)
	{
		SaveGameData->bHasSeenInitialIntro = true;
	}
}

void UPrologueGameInstance::TriggerDialogueCutScene(FName TriggerID, FName DialogueID)
{
	if (HasTriggerPlayed(TriggerID))
	{
		return;
	}

	MarkTriggerPlayed(TriggerID);

	ADialogueManager* DialogueManager = Cast<ADialogueManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ADialogueManager::StaticClass()));

	if (DialogueManager)
	{
		DialogueManager->StartDialogue(DialogueID);
	}
	else
	{
		LOG_SCREEN_R("Dialogue CutScene : Failed to find ADialogueManager");
	}
}

void UPrologueGameInstance::OpenStage()
{
	if (!LevelToLoad.IsEmpty() && SaveGameData)
	{
		UGameplayStatics::SaveGameToSlot(SaveGameData, SaveSlotName, UserIndex);

		UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelToLoad), true);

		LevelToLoad.Empty();
	}
}

void UPrologueGameInstance::OnIntroAnimationFinished()
{
	MarkInitialIntroSeen();

	OpenStage();
}

void UPrologueGameInstance::OnPreLoadMap(const FString& MapName)
{
	if (!bIsInitialLoadComplete)
	{
		return;
	}
	
	FLoadingScreenAttributes LoadingScreenAttributes;
	LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes = false;
	LoadingScreenAttributes.MinimumLoadingScreenDisplayTime = 5.f;
	LoadingScreenAttributes.WidgetLoadingScreen = CreateRandomLoadingWidget();

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);
}

void UPrologueGameInstance::OnDestinationWorldLoaded(UWorld* LoadedWorld)
{
	GetMoviePlayer()->StopMovie();

	if (!bIsInitialLoadComplete && LoadedWorld)
	{
		FString MainMenuMapName = TEXT("MainMenuMap");
		FString CurrentMapName = LoadedWorld->GetMapName();

		LOG_SCREEN_R("Map Loaded : %s, Comparing : %s", *CurrentMapName, *MainMenuMapName);
		
		if (CurrentMapName == MainMenuMapName)
		{
			LOG_SCREEN_R("Detected");
			bIsInitialLoadComplete = true;
		}
	}
}

TSharedPtr<SWidget> UPrologueGameInstance::CreateRandomLoadingWidget()
{
	if (LoadingScreenWidgets.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, LoadingScreenWidgets.Num() - 1);
		UClass* WidgetClass = LoadingScreenWidgets[RandomIndex].LoadSynchronous();

		if (WidgetClass)
		{
			UUserWidget* LoadingWidget = CreateWidget<UUserWidget>(this, WidgetClass);
			if (LoadingWidget)
			{
				return LoadingWidget->TakeWidget();
			}
		}
	}

	return FLoadingScreenAttributes::NewTestLoadingScreenWidget();
}

bool UPrologueGameInstance::HasAIDBeenDestroyed(FName AI_ID) const
{
	return DestroyedAI_IDs.Contains(AI_ID);
}

void UPrologueGameInstance::MarkAIDestroyed(FName AI_ID)
{
	if (!SaveGameData || HasAIDBeenDestroyed(AI_ID))
	{
		return;
	}

	DestroyedAI_IDs.Add(AI_ID);
	SaveGameData->DestroyedAI_IDs.Add(AI_ID);

	UGameplayStatics::SaveGameToSlot(SaveGameData, SaveSlotName, UserIndex);
}
