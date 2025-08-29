// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DialogueOverlapTrigger.generated.h"

class UBoxComponent;

UCLASS()
class PROLOGUE_API ADialogueOverlapTrigger : public AActor
{
	GENERATED_BODY()

public:
	ADialogueOverlapTrigger();

protected:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TriggerVolume;
	
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	FName TriggerID;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	FName StartDialogueID;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
