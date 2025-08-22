// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA_MontageAbility.h"
#include "GA_CommaSkill.generated.h"

class AComma;
/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_CommaSkill : public UGA_MontageAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TSubclassOf<UGameplayEffect> InvincibleEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline|Dash")
	TObjectPtr<class UCurveFloat> DashCurve;
	
protected:
	UFUNCTION()
	void OnDashCurveTick(float Alpha);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitStop")
	float HitStopDuration = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitStop")
	float HitStopTimeScale = 0.01f;
	
private:
	FVector BasePos;
	FVector TargetPos;

	FTimerHandle HitStopTimerHandle;

	void HitStop();
	void EndHitStop();

	bool bHitStopApplied = false;

	UPROPERTY()
	TSet<AActor*> HitActors;
};
