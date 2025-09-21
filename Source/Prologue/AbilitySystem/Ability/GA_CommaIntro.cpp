// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CommaIntro.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/Character/Player/Comma.h"
#include "Prologue/Game/PrologueGameInstance.h"

void UGA_CommaIntro::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UPrologueGameInstance* GameInstance = Cast<UPrologueGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		if (GameInstance->GetHasIntroDialoguePlayed())
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
			return;
		}
	}
	
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
		Comma->ShowCommaUI();

		if (UPrologueGameInstance* GameInstance = Cast<UPrologueGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
		{
			GameInstance->SetHasIntroDialoguePlayed(true);
		}
	}	
}
