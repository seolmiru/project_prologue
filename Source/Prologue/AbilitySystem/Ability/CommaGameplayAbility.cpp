// Fill out your copyright notice in the Description page of Project Settings.


#include "CommaGameplayAbility.h"

#include "Prologue/Character/Comma.h"
#include "Prologue/Controller/CommaController.h"

AComma* UCommaGameplayAbility::GetCommaFromActorInfo()
{
	if (!CachedCommaCharacter.IsValid())
	{
		CachedCommaCharacter = Cast<AComma>(CurrentActorInfo->AvatarActor);
	}

	return CachedCommaCharacter.IsValid() ? CachedCommaCharacter.Get() : nullptr;
}

ACommaController* UCommaGameplayAbility::GetControllerFromActorInfo()
{
	if (!CachedCommaCharacter.IsValid())
	{
		CachedCommaController = Cast<ACommaController>(CurrentActorInfo->PlayerController);
	}

	return CachedCommaCharacter.IsValid() ? CachedCommaController.Get() : nullptr;
}

UCommaCombatComponent* UCommaGameplayAbility::GetCombatComponentFromActorInfo()
{
	return GetCommaFromActorInfo()->GetCommaCombatComponent();
}
