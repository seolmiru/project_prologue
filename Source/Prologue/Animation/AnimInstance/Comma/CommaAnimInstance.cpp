// Fill out your copyright notice in the Description page of Project Settings.


#include "CommaAnimInstance.h"

#include "Prologue/Character/Player/Comma.h"

void UCommaAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (OwningCharacter)
	{
		OwningCharacter = Cast<AComma>(OwningCharacter);
	}
}

void UCommaAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}
