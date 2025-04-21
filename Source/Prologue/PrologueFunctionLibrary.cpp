// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/PrologueAbilitySystemComponent.h"

UPrologueAbilitySystemComponent* UPrologueFunctionLibrary::NativeGetPrologueASCFromActor(AActor* InActor)
{
	check(InActor);

	return CastChecked<UPrologueAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

void UPrologueFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	UPrologueAbilitySystemComponent* ASC = NativeGetPrologueASCFromActor(InActor);

	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

void UPrologueFunctionLibrary::RemoveGameplayFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UPrologueAbilitySystemComponent* ASC = NativeGetPrologueASCFromActor(InActor);

	if (ASC->HasMatchingGameplayTag(TagToRemove))
	{
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
}

bool UPrologueFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
	UPrologueAbilitySystemComponent* ASC = NativeGetPrologueASCFromActor(InActor);

	return ASC->HasMatchingGameplayTag(TagToCheck);
}

void UPrologueFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck,
	EPrologueConfirmType& OutConfirmType)
{
	OutConfirmType = NativeDoesActorHaveTag(InActor, TagToCheck) ? EPrologueConfirmType::Yes : EPrologueConfirmType::No;
}
