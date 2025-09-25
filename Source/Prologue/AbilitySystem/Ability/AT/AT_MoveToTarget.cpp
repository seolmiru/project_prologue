// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_MoveToTarget.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UAT_MoveToTarget* UAT_MoveToTarget::MoveToTarget(UGameplayAbility* OwningAbility, AActor* InTargetActor,
                                                 float Duration, float StopDistance)
{
	UAT_MoveToTarget* MyObj = NewAbilityTask<UAT_MoveToTarget>(OwningAbility);
	MyObj->TargetActor = InTargetActor;
	MyObj->Duration = FMath::Max(0.01f, Duration);
	MyObj->bTickingTask = true;
	MyObj->StopDistance = StopDistance;

	return MyObj;
}

void UAT_MoveToTarget::Activate()
{
	Super::Activate();

	AvatarActor = GetAvatarActor();
	if (!AvatarActor || !TargetActor)
	{
		EndTask();
		return;
	}
	
	StartLocation = AvatarActor->GetActorLocation();
	ElapsedTime = 0.f;
	
	ACharacter* Character = Cast<ACharacter>(AvatarActor);
	if (Character)
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_None);
	}

	float CapsuleRadius, TargetCapsuleRadius;
	float CapsuleHalfHeight, TargetCapsuleHalfHeight;

	Character->GetCapsuleComponent()->GetScaledCapsuleSize(CapsuleRadius, CapsuleHalfHeight);

	if (ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor))
	{
		TargetCharacter->GetCapsuleComponent()->GetScaledCapsuleSize(TargetCapsuleRadius, TargetCapsuleHalfHeight);
	}
	else
	{
		TargetCapsuleRadius = 50.f;
	}

	const FVector TargetInitialLocation = TargetActor->GetActorLocation();
	const FVector DirectionToTarget = (TargetInitialLocation - StartLocation).GetSafeNormal();

	const float Distance = CapsuleRadius + TargetCapsuleRadius + 10.f;

	FinalTargetLocation = TargetInitialLocation - DirectionToTarget * Distance;
}

void UAT_MoveToTarget::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	ElapsedTime += DeltaTime;
	float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.f, 1.f);

	FVector NewLocation = FMath::Lerp(StartLocation, FinalTargetLocation, Alpha);
	AvatarActor->SetActorLocation(NewLocation);

	if (Alpha >= 1.f)
	{
		if (ACharacter* Character = Cast<ACharacter>(GetAvatarActor()))
		{
			Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnFinished.Broadcast();
		}

		EndTask();
	}
}
