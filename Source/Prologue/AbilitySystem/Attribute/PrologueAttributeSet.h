// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "PrologueAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOutOfHealthDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOutOfToughnessDelegate);

/**
 * 
 */
UCLASS()
class PROLOGUE_API UPrologueAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPrologueAttributeSet();

	ATTRIBUTE_ACCESSORS(UPrologueAttributeSet, CurrentHealth);
	ATTRIBUTE_ACCESSORS(UPrologueAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UPrologueAttributeSet, Damage);
	ATTRIBUTE_ACCESSORS(UPrologueAttributeSet, CurrentToughness);
	ATTRIBUTE_ACCESSORS(UPrologueAttributeSet, MaxToughness);
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	mutable FOutOfHealthDelegate OnOutOfHealth;
	mutable FOutOfToughnessDelegate OnOutOfToughness;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CurrentHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Damage;

	UPROPERTY(BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CurrentToughness;

	UPROPERTY(BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxToughness;
	
	bool bOutOfHealth = false;

	bool bOutOfToughness = false;
};
