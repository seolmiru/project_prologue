// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "PrologueSkillAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class PROLOGUE_API UPrologueSkillAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPrologueSkillAttributeSet();

	ATTRIBUTE_ACCESSORS(UPrologueSkillAttributeSet, SwordSwitchAttackDamage);
	ATTRIBUTE_ACCESSORS(UPrologueSkillAttributeSet, MaxSwordSwitchAttackDamage);
	ATTRIBUTE_ACCESSORS(UPrologueSkillAttributeSet, SwordSwitchAttackRange);
	ATTRIBUTE_ACCESSORS(UPrologueSkillAttributeSet, MaxSwordSwitchAttackRange);
	ATTRIBUTE_ACCESSORS(UPrologueSkillAttributeSet, CurrentGauge);
	ATTRIBUTE_ACCESSORS(UPrologueSkillAttributeSet, MaxGauge);
	ATTRIBUTE_ACCESSORS(UPrologueSkillAttributeSet, CurrentHealPotion);
	ATTRIBUTE_ACCESSORS(UPrologueSkillAttributeSet, MaxHealPotion);
	ATTRIBUTE_ACCESSORS(UPrologueSkillAttributeSet, Currency);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SwordSwitchAttackDamage;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxSwordSwitchAttackDamage;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SwordSwitchAttackRange;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxSwordSwitchAttackRange;

	UPROPERTY(BlueprintReadOnly, Category = "OverClock", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CurrentGauge;
	
	UPROPERTY(BlueprintReadOnly, Category = "OverClock", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxGauge;

	UPROPERTY(BlueprintReadOnly, Category = "OverClock", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CurrentHealPotion;
	
	UPROPERTY(BlueprintReadOnly, Category = "OverClock", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealPotion;

	UPROPERTY(BlueprintReadOnly, Category = "OverClock", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Currency;
};
