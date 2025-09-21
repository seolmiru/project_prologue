// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueAbilitySystemComponent.h"

#include "Prologue/Prologue.h"


void UPrologueAbilitySystemComponent::ResetCooldownWithMatchingOwningTag(
	const FGameplayTagContainer CooldownTagContainer)
{
	// Query에 담겨져있는 Tag가 일치하는 Effect를 제거하여 Cooldown 초기화
	const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTagContainer);
	RemoveActiveEffects(Query);
}

void UPrologueAbilitySystemComponent::ReduceCooldownWithMatchingOwningTag(
	const FGameplayTagContainer CooldownTagContainer, const float ReductionAmount)
{
	if (ReductionAmount <= 0.f)
	{
		return;
	}

	const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTagContainer);
	TArray<FActiveGameplayEffectHandle> MatchingEffects = GetActiveEffects(Query);
	for (const FActiveGameplayEffectHandle& Handle : MatchingEffects)
	{
		if (const FActiveGameplayEffect* ActiveEffect = GetActiveGameplayEffect(Handle))
		{
			// Effect의 남아있는 지속 시간 가져오기
			const float RemainingDuration = ActiveEffect->GetTimeRemaining(GetWorld()->GetTimeSeconds());
			const float NewDuration = FMath::Max(RemainingDuration - ReductionAmount, 0.f);

			if (NewDuration > 0.f)
			{
				if (const TSubclassOf<UGameplayEffect> CooldownEffectClass = ActiveEffect->Spec.Def->GetClass())
				{
					FGameplayEffectSpecHandle NewSpecHandle = MakeOutgoingSpec(CooldownEffectClass, ActiveEffect->Spec.GetLevel(), ActiveEffect->Spec.GetEffectContext());
					if (NewSpecHandle.IsValid())
					{
						// Effect의 지속 시간과 Tag를 NewSpecHandle로 교체
						NewSpecHandle.Data->SetDuration(NewDuration, true);
						NewSpecHandle.Data->DynamicGrantedTags = ActiveEffect->Spec.DynamicGrantedTags;

						// 현재 적용되어 있는 Effect 삭제
						RemoveActiveGameplayEffect(Handle);

						// 새로운 Effect 적용
						ApplyGameplayEffectSpecToSelf(*NewSpecHandle.Data);
					}
				}
			}
		}
		else
		{
			RemoveActiveGameplayEffect(Handle);
		}
	}
}
