// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prologue/AnimInstance/PrologueCharacterAnimInstance.h"
#include "CommaAnimInstance.generated.h"

class AComma;
/**
 * 
 */
UCLASS()
class PROLOGUE_API UCommaAnimInstance : public UPrologueCharacterAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Refrences")
	AComma* OwningCommaCharacter;
};
