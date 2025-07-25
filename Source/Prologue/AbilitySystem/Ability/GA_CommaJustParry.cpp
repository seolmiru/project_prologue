// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CommaJustParry.h"

#include "AbilitySystemComponent.h"
#include "AT/AT_TickCurve.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/PrologueGameplayTags.h"

void UGA_CommaJustParry::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAT_TickCurve* TickCurveTask = UAT_TickCurve::CreateTask(this, SlowCurve);
	TickCurveTask->OnComplete.AddDynamic(this, &UGA_CommaJustParry::OnComplete);
	TickCurveTask->OnCurveTick.AddDynamic(this, &UGA_CommaJustParry::OnSlowCurveTick);
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_Parried);

	TickCurveTask->ReadyForActivation();
}

void UGA_CommaJustParry::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_CommaJustParry::OnComplete()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UGA_CommaJustParry::OnSlowCurveTick(float Alpha)
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), Alpha);
}
