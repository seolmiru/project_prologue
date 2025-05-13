// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA_MontageAbility.h"
#include "GA_EnemyDeath.generated.h"

class APrologueEnemyCharacter;
/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_EnemyDeath : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION(BlueprintPure)
	APrologueEnemyCharacter* GetEnemyCharacterFromActorInfo();
	
private:
	TWeakObjectPtr<APrologueEnemyCharacter> CachedEnemyCharacter;
};
