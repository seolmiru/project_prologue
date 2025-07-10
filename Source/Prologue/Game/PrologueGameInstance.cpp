// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueGameInstance.h"

#include "Blueprint/UserWidget.h"

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
}

void UPrologueGameInstance::OnPreLoadMap(const FString& MapName)
{
	FLoadingScreenAttributes LoadingScreenAttributes;
	LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes = true;
	LoadingScreenAttributes.MinimumLoadingScreenDisplayTime = 2.f;
	LoadingScreenAttributes.WidgetLoadingScreen = CreateRandomLoadingWidget();

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);
}

void UPrologueGameInstance::OnDestinationWorldLoaded(UWorld* LoadedWorld)
{
	GetMoviePlayer()->StopMovie();
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
