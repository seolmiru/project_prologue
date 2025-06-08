// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA_EnemyAttack.h"
#include "GA_MangoAttack.generated.h"

class UAT_TickSocketTrace;
/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_MangoAttack : public UGA_EnemyAttack
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnTraceResult(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION()
	void StartSocketTrace();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float AttackDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	FName StartSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	FName EndSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float TraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> AttackDamageEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> ToughnessDamageEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	bool bShowDebug = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	float TraceStartDelay = 0.3f;

	UPROPERTY()
	UAT_TickSocketTrace* SocketTraceTask;

	FTimerHandle TraceStartTimerHandle;
};
