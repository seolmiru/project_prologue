// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "../Prologue.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "PrologueCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class APrologueCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APrologueCharacter();
};

