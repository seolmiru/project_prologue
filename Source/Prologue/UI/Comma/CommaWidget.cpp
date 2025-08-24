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

void UCommaWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ASC && CooldownTag.IsValid())
	{
		FGameplayEffectQuery Query;
		Query.OwningTagQuery = FGameplayTagQuery::MakeQuery_MatchAllTags(FGameplayTagContainer(CooldownTag));

		TArray<FActiveGameplayEffectHandle> ActiveEffects = ASC->GetActiveEffects(Query);

		if (ActiveEffects.Num() > 0)
		{
			FActiveGameplayEffectHandle CooldownEffectHandle = ActiveEffects[0];

			const FActiveGameplayEffect* CooldownEffect = ASC->GetActiveGameplayEffect(CooldownEffectHandle);
			if (CooldownEffect)
			{
				const float TimeRemaining = CooldownEffect->GetTimeRemaining(GetWorld()->TimeSeconds);
				const float TotalDuration = CooldownEffect->GetDuration();

				if (TotalDuration > 0.f)
				{
					const float Percent = 1.f - (TimeRemaining / TotalDuration);
					UpdateGaugePercent(Percent);
				}
			}
		}
		else
		{
			UpdateGaugePercent(1.f);
		}
	}
}

void UCommaWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	if (ASC)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UPrologueAttributeSet::GetCurrentHealthAttribute()).AddUObject(this, &UCommaWidget::OnCurrentHealthChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UPrologueAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UCommaWidget::OnMaxHealthChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UPrologueSkillAttributeSet::GetCurrentHealPotionAttribute()).AddUObject(this, &UCommaWidget::OnCurrentHealPotionChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UPrologueSkillAttributeSet::GetCurrencyAttribute()).AddUObject(this, &UCommaWidget::OnCurrencyChanged);
			
		if (CooldownTag.IsValid())
		{
			ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UCommaWidget::OnCooldownTagChanged);
		}
		
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

void UCommaWidget::OnCooldownTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	const bool bIsOnCooldown = NewCount > 0;

	if (!bIsOnCooldown)
	{
		UpdateGaugePercent(1.f);
	}
}

void UCommaWidget::OnCurrentHealPotionChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentHealPotion = ChangeData.NewValue;
}

void UCommaWidget::OnCurrencyChanged(const FOnAttributeChangeData& ChangeData)
{
	Currency = ChangeData.NewValue;
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

void UCommaWidget::UpdateGaugePercent(float Percent)
{
	if (GaugeMaterialInstance)
	{
		GaugeMaterialInstance->SetScalarParameterValue(TEXT("Percent"), Percent);
	}
}
