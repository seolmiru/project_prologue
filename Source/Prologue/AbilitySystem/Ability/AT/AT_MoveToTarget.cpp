// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_MoveToTarget.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UAT_MoveToTarget* UAT_MoveToTarget::MoveToTarget(UGameplayAbility* OwningAbility, FVector TargetLocation,
                                                 float Duration)
{
	UAT_MoveToTarget* MyObj = NewAbilityTask<UAT_MoveToTarget>(OwningAbility);
	MyObj->TargetLocation = TargetLocation;
	MyObj->Duration = FMath::Max(0.01f, Duration);
	MyObj->bTickingTask = true;

	return MyObj;
}

void UAT_MoveToTarget::Activate()
{
	Super::Activate();

	StartLocation = GetAvatarActor()->GetActorLocation();
	ElapsedTime = 0.f;

	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActor()))
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_None);
	}
}

void UAT_MoveToTarget::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	ElapsedTime += DeltaTime;
	float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.f, 1.f);

	FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
	GetAvatarActor()->SetActorLocation(NewLocation);

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
