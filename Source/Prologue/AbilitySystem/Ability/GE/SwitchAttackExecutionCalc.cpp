// Fill out your copyright notice in the Description page of Project Settings.


#include "SwitchAttackExecutionCalc.h"

#include "AbilitySystemComponent.h"
#include "Prologue/AbilitySystem/PrologueAttributeSet.h"

void USwitchAttackExecutionCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                        FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	if (SourceASC && TargetASC)
	{
		AActor* SourceActor = SourceASC->GetAvatarActor();
		AActor* TargetActor = TargetASC->GetAvatarActor();

		if (SourceActor && TargetActor)
		{
			const float MaxDamageRange = SourceASC->GetNumericAttributeBase(UPrologueAttributeSet::GetSwordSwitchAttackRangeAttribute());
			const float Distance = SourceActor->GetDistanceTo(TargetActor);

			float Damage = 0.f;

			if (Distance <= MaxDamageRange)
			{
				if (Distance <= 300.f)
				{
					Damage = 100.f;
				}
				else
				{
					Damage = 40.f;
				}
			}

			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UPrologueAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, Damage));
		}
	}
}
