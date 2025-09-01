// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingDamage.h"

#include "AbilitySystemComponent.h"
#include "Prologue/AbilitySystem/Attribute/PrologueAttributeSet.h"

void UFloatingDamage::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	if (ASC)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UPrologueAttributeSet::GetDamageAttribute()).AddUObject(this, &UFloatingDamage::OnDamageChanged);
	}
}

void UFloatingDamage::OnDamageChanged(const FOnAttributeChangeData& ChangeData)
{
	Damage = ChangeData.NewValue;
}
