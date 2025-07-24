// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA_MontageAbility.h"
#include "GA_CommaParry.generated.h"

class AComma;
/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_CommaParry : public UGA_MontageAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	/*UFUNCTION()
	void OnCurveTick(float Alpha);*/
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TSubclassOf<UGameplayEffect> ParryEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	TObjectPtr<class UCurveFloat> Curve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parry")
	float ParryRadius = 300.f;
	
private:
	void Deflect(AComma* Comma);
};
