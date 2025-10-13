// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueGameInstance.h"

#include "PrologueSaveGame.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/Prologue.h"
#include "Prologue/PrologueObject/CenterHub.h"

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
	}
}

void UPrologueGameInstance::SetHasIntroDialoguePlayed(bool bPlayed)
{
	if (SaveGameData)
	{
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
	bHasIntroDialoguePlayed = false;

	UGameplayStatics::SaveGameToSlot(SaveGameData, SaveSlotName, UserIndex);

	UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName), true);
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
		LOG_SCREEN_R("CenterHub is Register");
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
