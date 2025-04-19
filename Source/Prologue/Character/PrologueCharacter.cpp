// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrologueCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Prologue/AbilitySystem/PrologueAbilitySystemComponent.h"
#include "Prologue/AbilitySystem/PrologueAttributeSet.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// APrologueCharacter

APrologueCharacter::APrologueCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetMesh()->bReceivesDecals = false;

	PrologueAbilitySystemComponent = CreateDefaultSubobject<UPrologueAbilitySystemComponent>(TEXT("PrologueAbilitySystemComponent"));

	PrologueAttributeSet = CreateDefaultSubobject<UPrologueAttributeSet>(TEXT("PrologueAttributeSet"));
}

UAbilitySystemComponent* APrologueCharacter::GetAbilitySystemComponent() const
{
	return GetPrologueAbilitySystemComponent();
}

UPawnCombatComponent* APrologueCharacter::GetPawnCombatComponent() const
{
	return nullptr;
}

void APrologueCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (PrologueAbilitySystemComponent)
	{
		PrologueAbilitySystemComponent->InitAbilityActorInfo(this, this);

		ensureMsgf(!CharacterStartUpData.IsNull(), TEXT("Forgot to assign start up data to %s"), *GetName());
	}
}
