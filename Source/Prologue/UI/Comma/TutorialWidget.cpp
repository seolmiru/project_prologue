// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialWidget.h"

#include "AbilitySystemComponent.h"
#include "Components/TextBlock.h"
#include "Prologue/Character/Player/Comma.h"

void UTutorialWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AComma* Comma = Cast<AComma>(PC->GetPawn()))
		{
			UAbilitySystemComponent* ASC = Comma->GetAbilitySystemComponent();

			ASC->AbilityActivatedCallbacks.AddUObject(this, &UTutorialWidget::OnAbilityActivated);
		}
	}
}

void UTutorialWidget::OnAbilityActivated(UGameplayAbility* ActivatedAbility)
{
	if (ActivatedAbility->AbilityTags.HasTag(FGameplayTag::RequestGameplayTag("Comma.Ability.Attack.Sword")))
	{
		LeftClickText->SetOpacity(0.5f);
	}

	if (ActivatedAbility->AbilityTags.HasTag(FGameplayTag::RequestGameplayTag("Comma.Ability.Skill")))
	{
		RightClickText->SetOpacity(0.5f);
	}
}
