// Fill out your copyright notice in the Description page of Project Settings.


#include "CommaCombatComponent.h"

#include "Prologue/Weapon/PrologueCommaWeapon.h"


APrologueCommaWeapon* UCommaCombatComponent::GetCommaCarriedWeaponByTag(FGameplayTag InWeaponTag) const
{
	return Cast<APrologueCommaWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}

APrologueCommaWeapon* UCommaCombatComponent::GetCommaCurrentEquippedWeapon() const
{
	return Cast<APrologueCommaWeapon>(GetCharacterCurrentEquippedWeapon());
}
