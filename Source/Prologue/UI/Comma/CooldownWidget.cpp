// Fill out your copyright notice in the Description page of Project Settings.


#include "CooldownWidget.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Prologue/Prologue.h"

void UCooldownWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetAbilitySystemComponent(GetOwningPlayer());
	
	if (CooldownGaugeMaterial && CooldownGaugeImage)
	{
		CooldownGaugeMaterialInstance = UMaterialInstanceDynamic::Create(CooldownGaugeMaterial, this);
		CooldownGaugeImage->SetBrush(UWidgetBlueprintLibrary::MakeBrushFromMaterial(CooldownGaugeMaterialInstance));
	}
}

void UCooldownWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (ASC && CooldownTag.IsValid())
	{
		FGameplayEffectQuery Query;
		Query.OwningTagQuery = FGameplayTagQuery::MakeQuery_MatchAllTags(FGameplayTagContainer(CooldownTag));

		TArray<FActiveGameplayEffectHandle> ActiveEffects = ASC->GetActiveEffects(Query);

		if (ActiveEffects.Num() > 0)
		{
			LOG_SCREEN_R("CooldownWidget : Effect Found : %d", ActiveEffects.Num());
			
			FActiveGameplayEffectHandle CooldownEffectHandle = ActiveEffects[0];

			const FActiveGameplayEffect* CooldownEffect = ASC->GetActiveGameplayEffect(CooldownEffectHandle);
			if (CooldownEffect)
			{
				const float TimeRemaining = CooldownEffect->GetTimeRemaining(GetWorld()->TimeSeconds);
				const float TotalDuration = CooldownEffect->GetDuration();
				LOG_SCREEN_R("CooldownWidget : Effect Duration : %f", TotalDuration);

				if (TotalDuration > 0.f)
				{
					const float Percent = 1.f - (TimeRemaining / TotalDuration);
					UpdateCooldownGaugePercent(Percent);
				}
			}
		}
		else
		{
			LOG_SCREEN_R("CooldownWidget : SSival");
			UpdateCooldownGaugePercent(1.f);
		}
	}
}

void UCooldownWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	if (ASC)
	{
		if (CooldownTag.IsValid())
		{
			ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UCooldownWidget::OnCooldownTagChanged);
		}
	}
}

void UCooldownWidget::OnCooldownTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	const bool bIsOnCooldown = NewCount > 0;

	if (!bIsOnCooldown)
	{
		UpdateCooldownGaugePercent(1.f);
	}
}

void UCooldownWidget::UpdateCooldownGaugePercent(float Percent)
{
	if (CooldownGaugeMaterialInstance)
	{
		CooldownGaugeMaterialInstance->SetScalarParameterValue(TEXT("Percent"), Percent);
	}
}
