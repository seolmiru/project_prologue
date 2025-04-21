// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "../Prologue.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
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

	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;

protected:
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "GAS")
	void InputGAS(const FGameplayTag Tag);
	
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};

