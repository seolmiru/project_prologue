// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackTutorialWidget.h"

#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/Character/Player/Comma.h"

void UAttackTutorialWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (CheckIconImage)
	{
		CheckIconImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if (bIsCompleted)
	{
		bIsCompleted = false;
	}
}

void UAttackTutorialWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), CustomTimeDilation);

	if (APlayerController* Controller = GetOwningPlayer())
	{
		if (AComma* Comma = Cast<AComma>(Controller->GetPawn()))
		{
			UAbilitySystemComponent* ASC = Comma->GetAbilitySystemComponent();

			ASC->AbilityActivatedCallbacks.AddUObject(this, &UAttackTutorialWidget::OnAbilityActivated);
		}
	}
}

void UAttackTutorialWidget::OnAbilityActivated(UGameplayAbility* ActivatedAbility)
{
	if (ActivatedAbility->GetAssetTags().HasTag(FGameplayTag::RequestGameplayTag("Comma.Ability.Attack.Sword")))
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeDilation);

		if (WatchIconImage)
		{
			WatchIconImage->SetVisibility(ESlateVisibility::Hidden);

			CheckIconImage->SetVisibility(ESlateVisibility::Visible);

			bIsCompleted = true;
		}
	}
}
