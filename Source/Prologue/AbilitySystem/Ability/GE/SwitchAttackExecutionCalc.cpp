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
			const float MaxDamage = SourceASC->GetNumericAttributeBase(UPrologueAttributeSet::GetSwordSwitchAttackDamageAttribute());
			const float Distance = FMath::Clamp(SourceActor->GetDistanceTo(TargetActor), 0.0f, MaxDamageRange);
			const float InvDamageRatio = 1.0f - Distance / MaxDamageRange;
			float Damage = FMath::RoundToFloat(InvDamageRatio * MaxDamage * 10.f) / 10.f;

			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UPrologueAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, Damage));
		}
	}
}
