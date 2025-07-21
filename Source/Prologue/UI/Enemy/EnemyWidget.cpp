// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWidget.h"

#include "Components/ProgressBar.h"

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
			DelayedHealth = CurrentHealth;
		}

		if (DelayedHealthBar)
		{
			float HealthPercent = CurrentMaxHealth > 0 ? CurrentHealth / CurrentMaxHealth : 0.f;
			DelayedHealthBar->SetPercent(HealthPercent);
		}
	}
}

void UEnemyWidget::OnHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	float OldHealth = CurrentHealth;
	CurrentHealth = ChangeData.NewValue;

	// 체력이 감소되면 체력 감소 애니메이션 시작
	if (CurrentHealth < OldHealth)
	{
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(DelayedHealthTimerHandle);
			GetWorld()->GetTimerManager().ClearTimer(HealthAnimationTimerHandle);
		}

		DelayedHealth = OldHealth;

		StartHealthBarAnimation();
	}
}

void UEnemyWidget::OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMaxHealth = ChangeData.NewValue;

	if (DelayedHealthBar)
	{
		float DelayedPercent = CurrentMaxHealth > 0 ? DelayedHealth / CurrentMaxHealth : 0.f;
		DelayedHealthBar->SetPercent(DelayedPercent);
	}
}

void UEnemyWidget::UpdateDelayedHealthBar()
{
	float DeltaTime = 0.016f;
	float HealthDiff = DelayedHealth - CurrentHealth;

	if (FMath::Abs(HealthDiff) > 0.1f)
	{
		DelayedHealth -= HealthDiff * AnimationSpeed * DeltaTime;
		
		if (DelayedHealthBar)
		{
			float DelayedPercent = CurrentMaxHealth > 0 ? DelayedHealth / CurrentMaxHealth : 0.f;
			DelayedHealthBar->SetPercent(DelayedPercent);
		}
	}
	else
	{
		// 애니메이션 완료
		DelayedHealth = CurrentHealth;
		if (DelayedHealthBar)
		{
			float HealthPercent = CurrentMaxHealth > 0 ? CurrentHealth / CurrentMaxHealth : 0.f;
			DelayedHealthBar->SetPercent(HealthPercent);
		}

		GetWorld()->GetTimerManager().ClearTimer(DelayedHealthTimerHandle);
	}
}

void UEnemyWidget::StartHealthBarAnimation()
{
	// DelayAnimation의 값만큼 기다렸다가 애니메이션 시작
	GetWorld()->GetTimerManager().SetTimer(
		DelayedHealthTimerHandle,
		this,
		&UEnemyWidget::UpdateDelayedHealthBar,
		0.016f,
		true,
		DelayAnimation
	);
}
