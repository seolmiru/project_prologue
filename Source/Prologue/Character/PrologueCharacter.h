// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "../Prologue.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "Prologue/Interface/PawnCombatInterface.h"
#include "PrologueCharacter.generated.h"

class UPawnCombatComponent;
class UDataAsset_StartUpDataBase;
class UPrologueAttributeSet;
class UPrologueAbilitySystemComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class APrologueCharacter : public ACharacter, public IAbilitySystemInterface, public IPawnCombatInterface
{
	GENERATED_BODY()

public:
	APrologueCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;

protected:
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UPrologueAbilitySystemComponent> PrologueAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	UPrologueAttributeSet* PrologueAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	TSoftObjectPtr<UDataAsset_StartUpDataBase> CharacterStartUpData;

public:
	FORCEINLINE UPrologueAbilitySystemComponent* GetPrologueAbilitySystemComponent() const { return PrologueAbilitySystemComponent; }

	FORCEINLINE UPrologueAttributeSet* GetPrologueAttribute() const { return PrologueAttributeSet; }
};

