// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_EnemyCharge.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_EnemyCharge : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_EnemyCharge();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnCurveTick(float Alpha);
		
protected:
	UFUNCTION()
	virtual void OnComplete();

	UFUNCTION()
	virtual void OnInterrupted();
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> AnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	TObjectPtr<class UCurveFloat> ChargeCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveLength = 300.f;

private:
	FVector BasePos;
	FVector TargetPos;
};
