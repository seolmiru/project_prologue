// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA_MontageAbility.h"
#include "GA_MangoLaserAttack.generated.h"

class UAT_TickBoxTrace;
/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_MangoLaserAttack : public UGA_MontageAbility
{
	GENERATED_BODY()

public:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UFUNCTION()
	void OnTraceFinished();
    
	UFUNCTION()
	void OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION()
	void StartBoxTrace();

private:
	UPROPERTY()
	UAT_TickBoxTrace* BoxTraceTask = nullptr;
    
	FTimerHandle LaserTimerHandle;
	FTimerHandle TraceDelayTimerHandle;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	float LaserDuration = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	float TraceDelay = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	FVector LaserBoxHalfSize = FVector(50.f, 50.f, 50.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	float LaserLength = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	float DamageTickInterval = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	bool bShowDebugTrace = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
