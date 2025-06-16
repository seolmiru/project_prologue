// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CommaDeath.h"

#include "Prologue/Character/Player/Comma.h"
#include "Prologue/Controller/CommaController.h"

AComma* UGA_CommaDeath::GetCommaCharacterFromActorInfo()
{
	if (!CachedCommaCharacter.IsValid())
	{
		CachedCommaCharacter = Cast<AComma>(CurrentActorInfo->AvatarActor);
	}

	return CachedCommaCharacter.IsValid() ? CachedCommaCharacter.Get() : nullptr;
}

ACommaController* UGA_CommaDeath::GetCommaControllerFromActorInfo()
{
	if (!CachedCommaController.IsValid())
	{
		CachedCommaController = Cast<ACommaController>(CurrentActorInfo->PlayerController);
	}

	return CachedCommaController.IsValid() ? CachedCommaController.Get() : nullptr;
}
