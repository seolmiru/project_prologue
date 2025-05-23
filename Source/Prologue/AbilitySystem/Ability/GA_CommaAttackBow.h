// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GA_CommaAttackBow.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_CommaAttackBow : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_CommaAttackBow();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnComplete();

	UFUNCTION()
	void OnInterrupted();

	UFUNCTION()
	void HandleEnableComboInputEvent(FGameplayEventData Payload);

	UFUNCTION()
	void HandleDisableComboInputEvent(FGameplayEventData Payload);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = true))
	TMap<int32, UAnimMontage*> ComboMontageMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TSubclassOf<UGameplayEffect> SwitchAttackEffectClass;
 
	UPROPERTY()
	AActor* TargetActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float SphereRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	int32 CurrentComboCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	FTimerHandle ComboTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Combo")
	FGameplayTag EnableComboInputTag;

	UPROPERTY(EditAnywhere, Category = "Combo")
	FGameplayTag DisableComboInputTag;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> EnableComboInputEventTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> DisableComboInputEventTask;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PerfectShot")
	FGameplayTag PerfectShotReadyTag;

	UPROPERTY()
	bool bComboInputActivate = false;

	uint8 EffectCount = 0;

	bool HasNextComboInput = false;
};
