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

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	TSubclassOf<UDialogueWidget> DialogueWidgetClass;

	UPROPERTY()
	TObjectPtr<UDialogueWidget> DialogueWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> DialogueInputEAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> DialogueInputSpaceAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> DialogueInputLeftButtonAction;
	
	UFUNCTION()
	void HandleDialogueInput();

	UFUNCTION()
	void OnDialogueCompleted();

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	bool bDisablePlayerInputDuringDialogue = true;

	UPROPERTY()
	TObjectPtr<UDialogueWidget> CurrentDialogueWidget;

	UFUNCTION()
	void HandleOpenWidgetRequest(TSoftClassPtr<UUserWidget> WidgetClass);
	
private:
	UPROPERTY()
	bool bIsDialogueActive = false;

	UPROPERTY()
	uint32 DialogueInputBindingHandle;
};
