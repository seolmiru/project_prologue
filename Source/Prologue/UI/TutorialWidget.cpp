// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialWidget.h"

#include "MediaPlayer.h"
#include "MediaTexture.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Prologue/Prologue.h"

void UTutorialWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!MediaPlayer)
	{
		MediaPlayer = NewObject<UMediaPlayer>(this);
		MediaPlayer->OnMediaOpenFailed.AddDynamic(this, &UTutorialWidget::OnMediaOpenFailed);
		MediaPlayer->OnMediaOpened.AddDynamic(this, &UTutorialWidget::OnMediaOpened);
		MediaPlayer->SetLooping(true);
	}

	if (!MediaTexture && MediaPlayer)
	{
		MediaTexture = NewObject<UMediaTexture>(this);
		MediaTexture->SetMediaPlayer(MediaPlayer);
		MediaTexture->UpdateResource();
	}
}

void UTutorialWidget::NativeDestruct()
{
	if (MediaPlayer)
	{
		MediaPlayer->Close();
	}
	
	Super::NativeDestruct();
}

void UTutorialWidget::SetTutorialData(const FTutorialData& InTutorialData)
{
	TutorialData = InTutorialData;

	if (TitleText)
	{
		TitleText->SetText(TutorialData.TutorialTitle);
	}

	if (DescriptionText)
	{
		DescriptionText->SetText(TutorialData.TutorialDescription);
	}

	if (MediaPlayer && TutorialData.TutorialVideo)
	{
		MediaPlayer->OpenSource(TutorialData.TutorialVideo);
	}
}

void UTutorialWidget::OnMediaOpenFailed(FString FailedUrl)
{
	LOG_SCREEN_R("Failed to open tutorial vide : %s", *FailedUrl);
}

void UTutorialWidget::OnMediaOpened(FString OpenedUrl)
{
	if (VideoDisplay && MediaTexture)
	{
		UMaterialInstanceDynamic* VideoMaterial = UMaterialInstanceDynamic::Create(
			LoadObject<UMaterial>(nullptr, TEXT("/Engine/EngineResources/DefaultTextureMaterial")),
			this
		);

		if (VideoMaterial)
		{
			VideoMaterial->SetTextureParameterValue(FName("Texture"), MediaTexture);
			VideoDisplay->SetBrushFromMaterial(VideoMaterial);
		}

		MediaPlayer->Play();
	}
}
