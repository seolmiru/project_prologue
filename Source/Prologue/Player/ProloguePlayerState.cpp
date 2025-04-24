// Fill out your copyright notice in the Description page of Project Settings.


#include "ProloguePlayerState.h"

#include "AbilitySystemComponent.h"
#include "Prologue/AbilitySystem/PrologueAttributeSet.h"

AProloguePlayerState::AProloguePlayerState()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AttributeSet = CreateDefaultSubobject<UPrologueAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AProloguePlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
