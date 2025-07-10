// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueManager.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/Prologue.h"
#include "Prologue/UI/DialogueWidget.h"


ADialogueManager::ADialogueManager()
{
	PrimaryActorTick.bCanEverTick = false;

	DialogueInputBindingHandle = 0;
}

void ADialogueManager::BeginPlay()
{
	Super::BeginPlay();

	CommaController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (DialogueWidgetClass && CommaController)
	{
		DialogueWidget = CreateWidget<UDialogueWidget>(CommaController, DialogueWidgetClass);
		if (DialogueWidget)
		{
			DialogueWidget->AddToViewport(10);
			DialogueWidget->OnDialogueCompleted.AddDynamic(this, &ADialogueManager::OnDialogueCompleted);
		}
	}

	SetupInputComponent();
}

void ADialogueManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (DialogueWidget)
	{
		DialogueWidget->RemoveFromParent();
		DialogueWidget = nullptr;
	}
	
	Super::EndPlay(EndPlayReason);
}

void ADialogueManager::StartDialogue(FName StartDialogueID)
{
	if (!DialogueWidget)
	{
		return;
	}

	bIsDialogueActive = true;

	DialogueWidget->StartDialogue(StartDialogueID);
}

void ADialogueManager::EndDialogue()
{
	CleanUpInputComponent();
	
	bIsDialogueActive = false;
}

void ADialogueManager::SetupInputComponent()
{
	if (!CommaController || !DialogueInputAction)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(CommaController->GetLocalPlayer()))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(CommaController->InputComponent))
		{
			DialogueInputBindingHandle = EnhancedInputComponent->BindAction(DialogueInputAction, ETriggerEvent::Started, this, &ADialogueManager::HandleDialogueInput).GetHandle();
		}
	}
}

void ADialogueManager::CleanUpInputComponent()
{
	if (!CommaController || !DialogueInputAction)
	{
		return;
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(CommaController->InputComponent))
	{
		if (DialogueInputBindingHandle != 0)
		{
			EnhancedInputComponent->RemoveBindingByHandle(DialogueInputBindingHandle);
			DialogueInputBindingHandle = 0;
		}
	}
}

void ADialogueManager::HandleDialogueInput()
{
	if (!bIsDialogueActive || !DialogueWidget)
	{
		return;
	}
	
	DialogueWidget->NextDialogue();
}

void ADialogueManager::OnDialogueCompleted()
{
	EndDialogue();
}

