// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingDamage.h"

#include "AbilitySystemComponent.h"
#include "Prologue/AbilitySystem/Attribute/PrologueAttributeSet.h"

void UFloatingDamage::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	if (ASC)
	{
		if (const UPrologueAttributeSet* AttributeSet = ASC->GetSet<UPrologueAttributeSet>())
		{
			Damage = AttributeSet->GetDamage();
		}

		UpdateDamageText();
		
		ASC->GetGameplayAttributeValueChangeDelegate(UPrologueAttributeSet::GetDamageAttribute()).AddUObject(this, &UFloatingDamage::OnDamageChanged);
	}
}

void UFloatingDamage::OnDamageChanged(const FOnAttributeChangeData& ChangeData)
{
	Damage = ChangeData.NewValue;

	UpdateDamageText();
}

void UFloatingDamage::UpdateDamageText()
{
	DamageText = FText::AsNumber(Damage);
}
