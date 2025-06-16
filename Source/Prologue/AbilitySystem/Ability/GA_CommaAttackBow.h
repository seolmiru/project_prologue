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
	void OnBlendOut();

	FName GetNextSection();
	void StartComboTimer();
	void CheckComboInput();

	void ResetComboCount();

	void EnableComboInput();

	void ProcessNextCombo();

protected:
	UPROPERTY()
	TObjectPtr<class UComboBowData> CurrentComboData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TSubclassOf<UGameplayEffect> SwitchAttackEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	uint8 CurrentCombo = 0;
	
	UPROPERTY()
	AActor* TargetActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float SphereRadius;
	
	FTimerHandle ComboTimerHandle;
	bool HasNextComboInput = false;

	FTimerHandle CurrentComboTimerHandle;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PerfectShot")
	bool bIsPerfectShotActive = false;
};
