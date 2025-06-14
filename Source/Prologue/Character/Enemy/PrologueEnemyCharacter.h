// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prologue/AbilitySystem/PrologueAttributeSet.h"
#include "Prologue/Character/PrologueCharacter.h"
#include "PrologueEnemyCharacter.generated.h"

USTRUCT(BlueprintType)
struct FWeightedAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AbilityTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.1"))
	float Weight = 1.f;	
};

class UEnemyWidget;
class UGameplayAbility;

UCLASS()
class PROLOGUE_API APrologueEnemyCharacter : public APrologueCharacter
{
	GENERATED_BODY()

public:
	APrologueEnemyCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	
	void OnDamageAttributeChanged(const FOnAttributeChangeData& Data);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StartUpData", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StartUpData", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGameplayEffect>> StartEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	UPrologueAttributeSet* Attributes;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> AttackMontage;

protected:
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool TryActivateRandomAbilityWithWeights(const TArray<FWeightedAbilityInfo>& WeightedAbilities);

private:
	FDelegateHandle DamageAttributeChangedHandle;
};
