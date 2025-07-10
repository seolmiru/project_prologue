// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Actor.h"
#include "DialogueManager.generated.h"

class UInputAction;
class UDialogueWidget;

UCLASS()
class PROLOGUE_API ADialogueManager : public AActor
{
	GENERATED_BODY()

public:
	ADialogueManager();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogue(FName StartDialogueID);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void EndDialogue();

	void SetupInputComponent();
	void CleanUpInputComponent();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	TSubclassOf<UDialogueWidget> DialogueWidgetClass;

	UPROPERTY()
	UDialogueWidget* DialogueWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* DialogueInputAction;

	UFUNCTION()
	void HandleDialogueInput();

	UFUNCTION()
	void OnDialogueCompleted();

	UPROPERTY()
	APlayerController* CommaController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	bool bDisablePlayerInputDuringDialogue = true;

private:
	UPROPERTY()
	bool bIsDialogueActive = false;

	UPROPERTY()
	uint32 DialogueInputBindingHandle;
};
