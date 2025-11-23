// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWidget.h"

#include "Components/ProgressBar.h"

void UEnemyWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsDelayedHealth)
	{
		DelayedHealthPercent = FMath::FInterpTo(DelayedHealthPercent, HealthPercent, InDeltaTime, AnimationSpeed);

		if (FMath::IsNearlyEqual(DelayedHealthPercent, HealthPercent))
		{
			DelayedHealthPercent = HealthPercent;
			bIsDelayedHealth = false;
		}
	}
}

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

			HealthPercent = CurrentMaxHealth > 0.f ? CurrentHealth / CurrentMaxHealth : 0.f;
			DelayedHealth = CurrentHealth;
		}
	}
}

void UEnemyWidget::OnHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	float OldHealth = CurrentHealth;
	CurrentHealth = ChangeData.NewValue;

	HealthPercent = CurrentMaxHealth > 0.f ? CurrentHealth / CurrentMaxHealth : 0.f;

	if (CurrentHealth < OldHealth)
	{
		bIsDelayedHealth = true;
	}
}

void UEnemyWidget::OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMaxHealth = ChangeData.NewValue;

	HealthPercent = CurrentMaxHealth > 0.f ? CurrentHealth / CurrentMaxHealth : 0.f;
	DelayedHealthPercent = HealthPercent;
	bIsDelayedHealth = false;
}
