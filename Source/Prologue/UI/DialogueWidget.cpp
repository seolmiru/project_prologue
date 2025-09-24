// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Prologue/Prologue.h"

void UDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ContinueText)
	{
		ContinueText->SetText(FText::FromString(TEXT("Press E")));
	}

	if (HourHand)
	{
		HourHand->SetVisibility(ESlateVisibility::Hidden);
	}

	if (MinuteHand)
	{
		MinuteHand->SetVisibility(ESlateVisibility::Hidden);
	}
	
	SetVisibility(ESlateVisibility::Collapsed);
}

void UDialogueWidget::NativeDestruct()
{
	GetWorld()->GetTimerManager().ClearTimer(TypewriterTimerHandle);

	Super::NativeDestruct();
}

void UDialogueWidget::StartDialogue(FName StartDialogueID)
{
	if (!DialogueDataTable)
	{
		LOG_SCREEN_R("DialogueDataTable is not set");
		return;
	}

	CurrentDialogueID = StartDialogueID;
	bIsDialogueActive = true;

	SetVisibility(ESlateVisibility::Visible);

	FDialogueData* DialogueData = DialogueDataTable->FindRow<FDialogueData>(CurrentDialogueID, TEXT(""));
	if (DialogueData)
	{
		SetCurrentDialogue(*DialogueData);
	}
	else
	{
		LOG_SCREEN_R("Failed to find dialogue data for ID : %s", *CurrentDialogueID.ToString());
		EndDialogue();
	}
}

void UDialogueWidget::NextDialogue()
{
	if (!bIsDialogueActive)
	{
		return;
	}

	if (GetWorld()->GetTimerManager().IsTimerActive(TypewriterTimerHandle))
	{
		CompleteTypewriter();
		return;
	}

	FDialogueData* CurrentDialogue = DialogueDataTable->FindRow<FDialogueData>(CurrentDialogueID, TEXT(""));
	if (!CurrentDialogue)
	{
		EndDialogue();
		return;
	}

	if (CurrentDialogue->DialogueType == EDialogueType::End)
	{
		EndDialogue();
		return;
	}

	if (!CurrentDialogue->NextDialogueID.IsNone())
	{
		CurrentDialogueID = CurrentDialogue->NextDialogueID;
		FDialogueData* NextDialogue = DialogueDataTable->FindRow<FDialogueData>(CurrentDialogueID, TEXT(""));

		if (NextDialogue)
		{
			SetCurrentDialogue(*NextDialogue);
		}
		else
		{
			LOG_SCREEN_R("Failed to find next dialogue data for ID : %s", *CurrentDialogueID.ToString());
			EndDialogue();
		}
	}
	else
	{
		EndDialogue();
	}
}

void UDialogueWidget::EndDialogue()
{
	bIsDialogueActive = false;
	CurrentDialogueID = NAME_None;

	GetWorld()->GetTimerManager().ClearTimer(TypewriterTimerHandle);

	SetVisibility(ESlateVisibility::Collapsed);

	OnDialogueCompleted.Broadcast();

	LOG_SCREEN("Dialogue End");
}

void UDialogueWidget::SetCurrentDialogue(const FDialogueData& DialogueData)
{
	if (SpeakerNameText)
	{
		SpeakerNameText->SetText(FText::FromString(DialogueData.SpeakerName));

		UpdateCharacterIconStates(DialogueData.SpeakerName);
		
		FullDialogueText = DialogueData.DialogueText.ToString();

		FullDialogueText = FullDialogueText.Replace(TEXT("<br>"), TEXT("\n"));
		
		CurrentCharIndex = 0;

		if (DialogueText)
		{
			DialogueText->SetText(FText::GetEmpty());
		}

		GetWorld()->GetTimerManager().SetTimer(TypewriterTimerHandle, this, &UDialogueWidget::TypewriterEffect, TypewriterSpeed, true);
	}
}

void UDialogueWidget::TypewriterEffect()
{
	if (CurrentCharIndex >= FullDialogueText.Len())
	{
		CompleteTypewriter();
		return;
	}

	CurrentCharIndex++;
	FString CurrentText = FullDialogueText.Left(CurrentCharIndex);

	if (DialogueText)
	{
		DialogueText->SetText(FText::FromString(CurrentText));
	}
}

void UDialogueWidget::CompleteTypewriter()
{

	GetWorld()->GetTimerManager().ClearTimer(TypewriterTimerHandle);

	if (DialogueText)
	{
		DialogueText->SetText(FText::FromString(FullDialogueText));
	}

	CurrentCharIndex = FullDialogueText.Len();
}

void UDialogueWidget::UpdateCharacterIconStates(const FString& SpeakerName)
{
	bool bIsLeftSpeaker = SpeakerName.Contains(TEXT("분침"));

	if (bIsLeftSpeaker)
	{
		MinuteHand->SetVisibility(ESlateVisibility::Visible);
		HourHand->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		MinuteHand->SetVisibility(ESlateVisibility::Hidden);
		HourHand->SetVisibility(ESlateVisibility::Visible);
	}
}
