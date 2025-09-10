// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_EnemyDeath.h"

#include "AbilitySystemGlobals.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/Character/Enemy/PrologueEnemyCharacter.h"
#include "Prologue/Character/Player/Comma.h"

void UGA_EnemyDeath::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AComma* Comma = Cast<AComma>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (!Comma)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Comma);

	// 재화 증가 이펙트 적용
	if (ASC && AddCurrencyEffect)
	{
		FGameplayEffectContextHandle CurrencyContextHandle = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle CurrencySpecHandle = ASC->MakeOutgoingSpec(AddCurrencyEffect, 1.f, CurrencyContextHandle);
		
		if (CurrencySpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*CurrencySpecHandle.Data.Get());

		}
	}

	// 스킬 쿨다운 초기화 이펙트 적용
	if (ASC && ResetSkillCooldownEffect)
	{
		FGameplayEffectContextHandle SkillContextHandle = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle SkillSpecHandle = ASC->MakeOutgoingSpec(ResetSkillCooldownEffect, 1.f, SkillContextHandle);

		if (SkillSpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SkillSpecHandle.Data.Get());
		}
	}
	
	APrologueEnemyCharacter* Enemy = CastChecked<APrologueEnemyCharacter>(ActorInfo->AvatarActor.Get());

	Enemy->GetCharacterMovement()->SetMovementMode(MOVE_None);
}

APrologueEnemyCharacter* UGA_EnemyDeath::GetEnemyCharacterFromActorInfo()
{
	if (!CachedEnemyCharacter.IsValid())
	{
		CachedEnemyCharacter = Cast<APrologueEnemyCharacter>(CurrentActorInfo->AvatarActor);
	}

	return CachedEnemyCharacter.IsValid() ? CachedEnemyCharacter.Get() : nullptr;
}
