// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialTrigger.generated.h"

class ADialogueManager;

UCLASS()
class PROLOGUE_API ATutorialTrigger : public AActor
{
	GENERATED_BODY()

public:
	ATutorialTrigger();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	TSubclassOf<ADialogueManager> DialogueManagerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	FName StartDialogueID = "Intro_M_001";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	float StartDelay = 2.f;

	UPROPERTY()
	ADialogueManager* DialogueManager;

	FTimerHandle StartDialogueTimerHandle;

	UFUNCTION()
	void StartTutorialDialogue();
};
