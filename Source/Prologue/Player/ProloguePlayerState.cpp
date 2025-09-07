// Fill out your copyright notice in the Description page of Project Settings.


#include "ProloguePlayerState.h"

#include "AbilitySystemComponent.h"
#include "Prologue/AbilitySystem/PrologueAbilitySystemComponent.h"
#include "Prologue/AbilitySystem/Attribute/PrologueAttributeSet.h"
#include "Prologue/AbilitySystem/Attribute/PrologueSkillAttributeSet.h"

AProloguePlayerState::AProloguePlayerState()
{
	ASC = CreateDefaultSubobject<UPrologueAbilitySystemComponent>(TEXT("ASC"));
	AttributeSet = CreateDefaultSubobject<UPrologueAttributeSet>(TEXT("AttributeSet"));
	SkillAttributeSet = CreateDefaultSubobject<UPrologueSkillAttributeSet>(TEXT("SkillAttributeSet"));
}

UAbilitySystemComponent* AProloguePlayerState::GetAbilitySystemComponent() const
{
	return ASC.Get();
}
