// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueAttributeSet.h"

UPrologueAttributeSet::UPrologueAttributeSet()
{
}

void UPrologueAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}
