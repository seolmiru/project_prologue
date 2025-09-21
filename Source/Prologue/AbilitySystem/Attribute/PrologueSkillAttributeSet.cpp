// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueSkillAttributeSet.h"

UPrologueSkillAttributeSet::UPrologueSkillAttributeSet() :
	SwordSwitchAttackDamage(80.f),
	MaxSwordSwitchAttackDamage(500.f),
	SwordSwitchAttackRange(500.f),
	MaxSwordSwitchAttackRange(800.f)
{
	InitCurrentGauge(1.f);
	InitMaxGauge(1.f);
	InitCurrentHealPotion(1.f);
	InitMaxHealPotion(1.f);
	InitCurrency(0.0f);
}

void UPrologueSkillAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetSwordSwitchAttackRangeAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.1f, GetMaxSwordSwitchAttackRange());
	}
	else if (Attribute == GetSwordSwitchAttackDamageAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.1f, GetMaxSwordSwitchAttackDamage());
	}
}

bool UPrologueSkillAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	return Super::PreGameplayEffectExecute(Data);
}

void UPrologueSkillAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}
