// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWidget.h"

void UEnemyWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	if (ASC)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UPrologueAttributeSet::GetCurrentHealthAttribute()).AddUObject(this, &UEnemyWidget::OnHealthChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UPrologueAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UEnemyWidget::OnMaxHealthChanged);

		if (const UPrologueAttributeSet* CurrentAttributeSet = ASC->GetSet<UPrologueAttributeSet>())
		{
			CurrentHealth = CurrentAttributeSet->GetCurrentHealth();
			CurrentMaxHealth = CurrentAttributeSet->GetMaxHealth();
		}
	}
}

void UEnemyWidget::OnHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentHealth = ChangeData.NewValue;
}

void UEnemyWidget::OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMaxHealth = ChangeData.NewValue;
}
