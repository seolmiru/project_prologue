// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_WaitForTrace.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Prologue/AbilitySystem/Ability/TA/TA_Trace.h"

UAT_WaitForTrace::UAT_WaitForTrace()
{
}

UAT_WaitForTrace* UAT_WaitForTrace::CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<class ATA_Trace> TargetActorClass)
{
	UAT_WaitForTrace* NewTask = NewAbilityTask<UAT_WaitForTrace>(OwningAbility);
	NewTask->TargetActorClass = TargetActorClass;
	return NewTask;
}

void UAT_WaitForTrace::Activate()
{
	Super::Activate();

	SpawnAndInitializeTargetActor();
	FinalizeTargetActor();
	
	SetWaitingOnAvatar();
}

void UAT_WaitForTrace::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);

	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->Destroy();
	}
}

void UAT_WaitForTrace::SpawnAndInitializeTargetActor()
{
	SpawnedTargetActor = Cast<ATA_Trace>(Ability->GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(TargetActorClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->TargetDataReadyDelegate.AddUObject(this, &UAT_WaitForTrace::OnTargetDataReadyCallback);
	}
}

void UAT_WaitForTrace::FinalizeTargetActor()
{
	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		const FTransform SpawnTransform = ASC->GetAvatarActor()->GetTransform();
		SpawnedTargetActor->FinishSpawning(SpawnTransform);

		ASC->SpawnedTargetActors.Push(SpawnedTargetActor);
		SpawnedTargetActor->StartTargeting(Ability);
		SpawnedTargetActor->ConfirmTargeting();
	}
}

void UAT_WaitForTrace::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnComplete.Broadcast(DataHandle);
	}

	EndTask();
}
