// Fill out your copyright notice in the Description page of Project Settings.


#include "CommaWidget.h"

#include "Prologue/PrologueGameplayTags.h"

void UCommaWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ASC->HasMatchingGameplayTag(PrologueGameplayTags::Comma_State_Intro))
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		SetVisibility(ESlateVisibility::Visible);
	}
}

void UCommaWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	if (ASC)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UPrologueAttributeSet::GetCurrentHealthAttribute()).AddUObject(this, &UCommaWidget::OnCurrentHealthChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UPrologueAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UCommaWidget::OnMaxHealthChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UPrologueAttributeSet::GetCurrentGaugeAttribute()).AddUObject(this, &UCommaWidget::OnCurrentGaugeChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UPrologueAttributeSet::GetMaxGaugeAttribute()).AddUObject(this, &UCommaWidget::OnMaxGaugeChanged);

		if (const UPrologueAttributeSet* CurrentAttributeSet = ASC->GetSet<UPrologueAttributeSet>())
		{
			CurrentHealth = CurrentAttributeSet->GetCurrentHealth();
			CurrentMaxHealth = CurrentAttributeSet->GetMaxHealth();

			CurrentGauge = CurrentAttributeSet->GetCurrentGauge();
			CurrentMaxGauge = CurrentAttributeSet->GetMaxGauge();
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

void UCommaWidget::OnCurrentGaugeChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentGauge = ChangeData.NewValue;
}

void UCommaWidget::OnMaxGaugeChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMaxGauge = ChangeData.NewValue;
}
