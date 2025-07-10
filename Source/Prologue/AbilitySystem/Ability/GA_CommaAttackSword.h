// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_CommaAttackSword.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_CommaAttackSword : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_CommaAttackSword();

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

	UFUNCTION()
	void OnMoveToTargetFinished();
	
	FName GetNextSection();
	void StartComboTimer();
	void CheckComboInput();

	void ResetComboCount();

	void EnableComboInput();

	void ProcessNextCombo();

	AActor* FindNearestEnemyInFront();
	AActor* FindNearestEnemyInDirection(const FVector& SearchDirection);

	FVector GetMouseDirection() const;
	
	void StartAttackMontage();

protected:
	UPROPERTY()
	TObjectPtr<class UComboSwordData> CurrentComboData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TSubclassOf<UGameplayEffect> SwitchAttackEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	uint8 CurrentCombo = 0;
	
	FTimerHandle ComboTimerHandle;
	bool HasNextComboInput = false;

	FTimerHandle CurrentComboTimerHandle;

// 추격 시스템 관련 변수
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection", meta = (AllowPrivateAccess = true))
	float DetectionRange = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection", meta = (AllowPrivateAccess = true))
	float DetectionAngle = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection", meta = (AllowPrivateAccess = true))
	float DashDuration = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection", meta = (AllowPrivateAccess = true))
	float DashDistance = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = true))
	bool bShowDebugDetection = true;

private:
	UPROPERTY()
	class AComma* CachedComma = nullptr;

	FVector CachedAttackDirection;
};
