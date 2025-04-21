// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_Trace.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Prologue/AbilitySystem/Ability/TA/TA_Trace.h"

UAT_Trace::UAT_Trace()
{
}

UAT_Trace* UAT_Trace::CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<class ATA_Trace> TargetActorClass)
{
	UAT_Trace* NewTask = NewAbilityTask<UAT_Trace>(OwningAbility);
	NewTask->TargetActorClass = TargetActorClass;
	return NewTask;
}

void UAT_Trace::Activate()
{
	Super::Activate();

	SpawnAndInitializeTargetActor();
	FinalizeTargetActor();
	
	SetWaitingOnAvatar();
}

void UAT_Trace::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);

	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->Destroy();
	}
}

void UAT_Trace::SpawnAndInitializeTargetActor()
{
	SpawnedTargetActor = Cast<ATA_Trace>(Ability->GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(TargetActorClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->TargetDataReadyDelegate.AddUObject(this, &UAT_Trace::OnTargetDataReadyCallback);
	}
}

void UAT_Trace::FinalizeTargetActor()
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

void UAT_Trace::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnComplete.Broadcast(DataHandle);
	}

	EndTask();
}
