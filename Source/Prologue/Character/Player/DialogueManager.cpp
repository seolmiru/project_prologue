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

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (DialogueWidgetClass && PlayerController)
	{
		// 대사 위젯 띄우기
		DialogueWidget = CreateWidget<UDialogueWidget>(PlayerController, DialogueWidgetClass);
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

	if (bDisablePlayerInputDuringDialogue && PlayerController)
	{
		if (APawn* PlayerPawn = PlayerController->GetPawn())
		{
			PlayerPawn->DisableInput(PlayerController);
		}
	}
	
	DialogueWidget->StartDialogue(StartDialogueID);
}

void ADialogueManager::EndDialogue()
{
	bIsDialogueActive = false;

	if (bDisablePlayerInputDuringDialogue && PlayerController)
	{
		if (APawn* PlayerPawn = PlayerController->GetPawn())
		{
			PlayerPawn->EnableInput(PlayerController);
		}
	}

	CleanUpInputComponent();
}

void ADialogueManager::SetupInputComponent()
{
	if (!PlayerController || !DialogueInputAction)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			DialogueInputBindingHandle = EnhancedInputComponent->BindAction(DialogueInputAction, ETriggerEvent::Started, this, &ADialogueManager::HandleDialogueInput).GetHandle();
		}
	}
}

void ADialogueManager::CleanUpInputComponent()
{
	if (!PlayerController || !DialogueInputAction)
	{
		return;
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
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
	LOG_SCREEN("OnDialogueCompleted Called");
	EndDialogue();
}

