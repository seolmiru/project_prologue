// Fill out your copyright notice in the Description page of Project Settings.


#include "CommaWidget.h"

void UCommaWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	if (ASC)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UPrologueAttributeSet::GetCurrentHealthAttribute()).AddUObject(this, &UCommaWidget::OnCurrentHealthChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UPrologueAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UCommaWidget::OnMaxHealthChanged);

		if (const UPrologueAttributeSet* CurrentAttributeSet = ASC->GetSet<UPrologueAttributeSet>())
		{
			CurrentHealth = CurrentAttributeSet->GetCurrentHealth();
			CurrentMaxHealth = CurrentAttributeSet->GetMaxHealth();
		}
	}
}

void UCommaWidget::OnCurrentHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentHealth = ChangeData.NewValue;
}

void UCommaWidget::OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMaxHealth = ChangeData.NewValue;
}
