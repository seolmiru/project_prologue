// Fill out your copyright notice in the Description page of Project Settings.


#include "CommaWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Prologue/PrologueGameplayTags.h"

void UCommaWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (GaugeMaterial && GaugeImage)
	{
		GaugeMaterialInstance = UMaterialInstanceDynamic::Create(GaugeMaterial, this);
		GaugeImage->SetBrush(UWidgetBlueprintLibrary::MakeBrushFromMaterial(GaugeMaterialInstance));
	}

	if (RainbowGaugeMaterial && RainbowGaugeImage)
	{
		RainbowGaugeMaterialInstance = UMaterialInstanceDynamic::Create(RainbowGaugeMaterial, this);
		RainbowGaugeImage->SetBrush(UWidgetBlueprintLibrary::MakeBrushFromMaterial(RainbowGaugeMaterialInstance));
		RainbowGaugeImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if (PlayerIconOverClock)
	{
		PlayerIconOverClock->SetVisibility(ESlateVisibility::Hidden);
	}
	
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
		ASC->GetGameplayAttributeValueChangeDelegate(UPrologueSkillAttributeSet::GetCurrentGaugeAttribute()).AddUObject(this, &UCommaWidget::OnCurrentGaugeChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UPrologueSkillAttributeSet::GetMaxGaugeAttribute()).AddUObject(this, &UCommaWidget::OnMaxGaugeChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UPrologueSkillAttributeSet::GetCurrentHealPotionAttribute()).AddUObject(this, &UCommaWidget::OnCurrentHealPotionChanged);
		
		ASC->RegisterGameplayTagEvent(PrologueGameplayTags::Comma_State_OverClock, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UCommaWidget::OnOverClockTagChanged);

		const bool bIsOverClock = ASC->HasMatchingGameplayTag(PrologueGameplayTags::Comma_State_OverClock);
		const ESlateVisibility OverClockVisibility = bIsOverClock ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
		
		if (RainbowGaugeImage)
		{
			RainbowGaugeImage->SetVisibility(OverClockVisibility);
		}

		if (PlayerIconOverClock)
		{
			PlayerIconOverClock->SetVisibility(OverClockVisibility);
		}
		
		if (const UPrologueAttributeSet* CurrentAttributeSet = ASC->GetSet<UPrologueAttributeSet>())
		{
			if (const UPrologueSkillAttributeSet* SkillAttributeSet = ASC->GetSet<UPrologueSkillAttributeSet>())
			{
				CurrentHealth = CurrentAttributeSet->GetCurrentHealth();
				CurrentMaxHealth = CurrentAttributeSet->GetMaxHealth();

				CurrentGauge = SkillAttributeSet->GetCurrentGauge();
				CurrentMaxGauge = SkillAttributeSet->GetMaxGauge();
			}
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
	UpdateGaugePercent();
}

void UCommaWidget::OnMaxGaugeChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMaxGauge = ChangeData.NewValue;
	UpdateGaugePercent();
}

void UCommaWidget::OnCurrentHealPotionChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentHealPotion = ChangeData.NewValue;
}

void UCommaWidget::OnOverClockTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	const bool bIsOverClock = NewCount > 0;
	const ESlateVisibility OverClockVisibility = bIsOverClock ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
		
	if (RainbowGaugeImage)
	{
		RainbowGaugeImage->SetVisibility(OverClockVisibility);
	}

	if (PlayerIconOverClock)
	{
		PlayerIconOverClock->SetVisibility(OverClockVisibility);
	}
}

void UCommaWidget::UpdateGaugePercent()
{
	if (GaugeMaterialInstance && CurrentMaxGauge > 0.f)
	{
		float Percent = CurrentGauge / CurrentMaxGauge;
		GaugeMaterialInstance->SetScalarParameterValue(TEXT("Percent"), Percent);
	}
}
