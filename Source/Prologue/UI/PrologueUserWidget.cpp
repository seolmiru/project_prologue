// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueUserWidget.h"

#include "AbilitySystemBlueprintLibrary.h"

void UPrologueUserWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	if (IsValid(InOwner))
	{
		ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner);
	}
}

UAbilitySystemComponent* UPrologueUserWidget::GetAbilitySystemComponent() const
{
	return ASC;
}
