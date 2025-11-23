// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueIntroWidget.h"

#include "MediaPlayer.h"
#include "Prologue/Prologue.h"

void UPrologueIntroWidget::PlayIntroAnimation_Implementation()
{
	if (IntroMediaPlayer && IntroMediaPlayer->IsReady())
	{
		IntroMediaPlayer->Play();
	}
	else if (IntroMediaPlayer)
	{
		HandleAnimationFinished();	
	}
	else
	{
		HandleAnimationFinished();
	}
}

void UPrologueIntroWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IntroMediaPlayer)
	{
		IntroMediaPlayer->OnEndReached.AddDynamic(this, &UPrologueIntroWidget::HandleAnimationFinished);
	}
	else
	{
		LOG_SCREEN_R("Check IntroMediaPlayer");
	}

	bIsFocusable = true;

	SetFocus();

	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(this->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
	}
}

FReply UPrologueIntroWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape || InKeyEvent.GetKey() == EKeys::SpaceBar)
	{
		SkipIntro();
		return FReply::Handled();
	}
	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UPrologueIntroWidget::HandleAnimationFinished()
{
	LOG_SCREEN_R("NO PLEASE NO");
	
	if (bHasFinished)
	{
		return;
	}

	bHasFinished = true;

	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		FInputModeGameAndUI InputMode;
		PC->SetInputMode(InputMode);
	}

	if (IntroMediaPlayer)
	{
		IntroMediaPlayer->OnEndReached.RemoveDynamic(this, &UPrologueIntroWidget::HandleAnimationFinished);
	}

	if (OnIntroFinished.IsBound())
	{
		OnIntroFinished.Broadcast();
	}

	RemoveFromParent();
}

void UPrologueIntroWidget::SkipIntro()
{
	if (bHasFinished)
	{
		return;
	}

	if (IntroMediaPlayer && IntroMediaPlayer->IsPlaying())
	{
		IntroMediaPlayer->Close();
	}

	HandleAnimationFinished();
}
