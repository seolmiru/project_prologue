// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prologue/Character/PrologueCharacter.h"
#include "PrologueEnemyCharacter.generated.h"

class UGameplayAbility;

UCLASS()
class PROLOGUE_API APrologueEnemyCharacter : public APrologueCharacter
{
	GENERATED_BODY()

public:
	APrologueEnemyCharacter();

protected:
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StartUpData", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StartUpData", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGameplayEffect>> StartEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	UPrologueAttributeSet* Attributes;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> AttackMontage;
};
