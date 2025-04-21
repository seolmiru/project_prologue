// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrologueCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Prologue/PrologueGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Prologue/AbilitySystem/PrologueAttributeSet.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// APrologueCharacter

APrologueCharacter::APrologueCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetMesh()->bReceivesDecals = false;
}

UPawnCombatComponent* APrologueCharacter::GetPawnCombatComponent() const
{
	return nullptr;
}

void APrologueCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void APrologueCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APrologueCharacter::InputGAS(const FGameplayTag Tag)
{
	FGameplayTagContainer GameplayTags;

	LOG_SCREEN("%s", *Tag.ToString());
	
	GameplayTags.AddTag(Tag);
	if (ASC)
	{
		if (ASC->HasMatchingGameplayTag(PrologueGameplayTags::Comma_State_CanCancel))
		{
			FGameplayEventData PlayData;
			ASC->RemoveLooseGameplayTag(PrologueGameplayTags::Comma_State_CanCancel);
			InputGAS(PrologueGameplayTags::Comma_State_Cancel);
		}
		
		TArray<FGameplayAbilitySpec*> AbilitiesToActivatePtrs;
		ASC->GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTags, AbilitiesToActivatePtrs);
		if (AbilitiesToActivatePtrs.Num() < 1)
		{
			LOG_SCREEN("No Ability Specifications Found");
			return;
		}
		
		TArray<FGameplayAbilitySpec> AbilitiesToActivate;
		AbilitiesToActivate.Reserve(AbilitiesToActivatePtrs.Num());
		Algo::Transform(AbilitiesToActivatePtrs, AbilitiesToActivate, [](FGameplayAbilitySpec* SpecPtr) { return *SpecPtr; });
		
		for (FGameplayAbilitySpec& GameplayAbilitySpec : AbilitiesToActivate)
		{
			ensure(IsValid(GameplayAbilitySpec.Ability));
			if (GameplayAbilitySpec.IsActive())
			{
				ASC->AbilitySpecInputPressed(GameplayAbilitySpec);
			}
			else
			{
				ASC->TryActivateAbility(GameplayAbilitySpec.Handle);
			}
		}
	}
}

UAbilitySystemComponent* APrologueCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}
