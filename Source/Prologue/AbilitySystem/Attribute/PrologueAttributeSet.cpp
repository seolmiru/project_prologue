// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "../../PrologueGameplayTags.h"
#include "Prologue/Prologue.h"
#include "Prologue/Character/Player/Comma.h"

UPrologueAttributeSet::UPrologueAttributeSet()
{
	InitCurrentHealth(1.f);
	InitMaxHealth(1.f);
	InitDamage(1.f);
	InitCurrentToughness(1.f);
	InitMaxToughness(1.f);
}

void UPrologueAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetCurrentHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetDamageAttribute())
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

	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		if (Data.Target.HasMatchingGameplayTag(PrologueGameplayTags::Comma_State_Invincible))
		{
			return false;
		}

		if (Data.Target.HasMatchingGameplayTag(PrologueGameplayTags::Comma_State_Dashing))
		{
			FGameplayEventData PlayData;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Data.Target.GetAvatarActor(), PrologueGameplayTags::Comma_Event_JustDash, PlayData);
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Data.EffectSpec.GetContext().GetEffectCauser(), PrologueGameplayTags::Enemy_Event_Dashed, PlayData);
			LOG_SCREEN("%s", *Data.EffectSpec.GetContext().GetEffectCauser()->GetName());

			return false;
		}
	}

	if (Data.EvaluatedData.Attribute == GetCurrentToughnessAttribute())
	{
		if (Data.Target.HasMatchingGameplayTag(PrologueGameplayTags::Comma_State_Invincible))
		{
			return false;
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

		if (AActor* TargetActor = Data.Target.GetAvatarActor())
		{
			if (AComma* Comma = Cast<AComma>(TargetActor))
			{
				float HealthChange = Data.EvaluatedData.Magnitude;
				if (HealthChange < 0.f)
				{
					Comma->TriggerDamageEffect(FMath::Abs(HealthChange));
				}
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		LOG_SCREEN("Damage : %f", GetDamage());
		SetCurrentHealth(FMath::Clamp(GetCurrentHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
		SetDamage(0.0f);
	}

	if (Data.EvaluatedData.Attribute == GetCurrentToughnessAttribute())
	{
		LOG_SCREEN("Direct Toughness Access : %f", GetCurrentToughness());
		
		const float NewCurrentToughness = FMath::Clamp(GetCurrentToughness(), 0.f, GetMaxToughness());

		SetCurrentToughness(NewCurrentToughness);
	}
	
	if ((GetCurrentHealth() <= 0.0f) && !bOutOfHealth)
	{
		Data.Target.AddLooseGameplayTag(PrologueGameplayTags::Shared_State_IsDead);
		OnOutOfHealth.Broadcast();
	}

	if ((GetCurrentToughness() <= 0.0f) && !bOutOfToughness)
	{
		Data.Target.AddLooseGameplayTag(PrologueGameplayTags::Shared_State_IsOutOfToughness);
		OnOutOfToughness.Broadcast();
	}

	bOutOfHealth = (GetCurrentHealth() <= 0.0f);

	bOutOfToughness = (GetCurrentToughness() <= 0.0f);
}
