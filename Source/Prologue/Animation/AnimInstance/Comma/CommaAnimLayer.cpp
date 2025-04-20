// Fill out your copyright notice in the Description page of Project Settings.


#include "CommaAnimLayer.h"

#include "CommaAnimInstance.h"

UCommaAnimInstance* UCommaAnimLayer::GetCommaAnimInstance() const
{
	return Cast<UCommaAnimInstance>(GetOwningComponent()->GetAnimInstance());
}
