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

protected:
	virtual void BeginPlay() override;
};



