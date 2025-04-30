// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "../PrologueGameplayTags.h"
#include "Prologue/Prologue.h"

UPrologueAttributeSet::UPrologueAttributeSet()
{
	InitCurrentHealth(1.f);
	InitMaxHealth(1.f);
	InitCurrentGauge(1.f);
	InitMaxGauge(1.f);
	InitDamage(1.f);
	InitSwitchAttackDamage(1.f);
}

void UPrologueAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetDamageAttribute())
	{
		NewValue = NewValue < 0.0f ? 0.0f : NewValue;
	}
}

bool UPrologueAttributeSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			if (Data.Target.HasMatchingGameplayTag(PrologueGameplayTags::Comma_State_Invincible))
			{
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}
		}
	}

	return true;
}

void UPrologueAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumHealth = 0.0f;
	
	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		LOG_SCREEN("Direct Health Access : %f", GetCurrentHealth());
		SetCurrentHealth(FMath::Clamp(GetCurrentHealth(), MinimumHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		LOG_SCREEN("Damage : %f", GetDamage());
		SetCurrentHealth(FMath::Clamp(GetCurrentHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
		SetDamage(0.0f);
	}

	if ((GetCurrentHealth() <= 0.0f) && !bOufOfHealth)
	{
		Data.Target.AddLooseGameplayTag(PrologueGameplayTags::Comma_State_IsDead);
		OnOutOfHealth.Broadcast();
	}

	bOufOfHealth = (GetCurrentHealth() <= 0.0f);
}
