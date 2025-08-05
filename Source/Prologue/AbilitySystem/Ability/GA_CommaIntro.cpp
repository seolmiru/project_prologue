// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CommaIntro.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/WidgetComponent.h"
#include "Prologue/Character/Player/Comma.h"

void UGA_CommaIntro::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (AComma* Comma = Cast<AComma>(GetAvatarActorFromActorInfo()))
	{
		Comma->HideCommaUI();
	}
}

void UGA_CommaIntro::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (AComma* Comma = Cast<AComma>(GetAvatarActorFromActorInfo()))
	{
		if (UCommaWidget* CommaWidget = Comma->GetCommaWidget())
		{
			CommaWidget->SetVisibility(ESlateVisibility::Visible);
		}
		
		Comma->ShowCommaUI();
	}	
}
