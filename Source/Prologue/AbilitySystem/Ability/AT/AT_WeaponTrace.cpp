// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_WeaponTrace.h"

#include "Kismet/KismetSystemLibrary.h"

UAT_WeaponTrace::UAT_WeaponTrace()
{
	bTickingTask = true;
}

UAT_WeaponTrace* UAT_WeaponTrace::CreateWeaponTraceTask(UGameplayAbility* OwningAbility, FName TaskInstanceName,
                                                        UStaticMeshComponent* WeaponMesh, FName StartSocket, FName EndSocket)
{
	UAT_WeaponTrace* TraceTask = NewAbilityTask<UAT_WeaponTrace>(OwningAbility, TaskInstanceName);

	TraceTask->WeaponMesh = WeaponMesh;
	TraceTask->StartSocket = StartSocket;
	TraceTask->EndSocket = EndSocket;

	return TraceTask;
}

void UAT_WeaponTrace::Activate()
{
	Super::Activate();

	if (!WeaponMesh)
	{
		EndTask();
		return;
	}

	LastStartSocketLocation = WeaponMesh->GetSocketLocation(StartSocket);
	HitActors.Empty();
}

void UAT_WeaponTrace::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (!WeaponMesh)
	{
		EndTask();
		return;
	}

	const FVector CurrentStartSocketLocation = WeaponMesh->GetSocketLocation(StartSocket);
	const FVector CurrentEndSocketLocation = WeaponMesh->GetSocketLocation(EndSocket);

	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActor());

	UKismetSystemLibrary::SphereTraceMulti(
		this,
		LastStartSocketLocation,
		CurrentStartSocketLocation,
		TraceSphereRadius,
		UEngineTypes::ConvertToTraceType(TraceChannel),
		false,
		ActorsToIgnore,
		bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResults,
		true
	);

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && !HitActors.Contains(HitActor))
		{
			HitActors.Add(HitActor);
			OnHit.Broadcast(Hit);
		}
	}

	LastStartSocketLocation = CurrentStartSocketLocation;
}

void UAT_WeaponTrace::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}
