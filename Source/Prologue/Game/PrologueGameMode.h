// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PrologueGameMode.generated.h"

UCLASS(minimalapi)
class APrologueGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APrologueGameMode();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SpawnGuide();

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	TObjectPtr<class ATutorialGuide> TutorialGuideActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ATutorialGuide> TutorialGuideClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FVector GuideSpawnLocation = FVector(0.f, 0.f, 500.f);
};



