// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "../Prologue.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Prologue/Interface/PawnCombatInterface.h"
#include "PrologueCharacter.generated.h"

struct FOnAttributeChangeData;
class UGameplayEffect;
class UPawnCombatComponent;
class UDataAsset_StartUpDataBase;
class UPrologueAttributeSet;
class UPrologueAbilitySystemComponent;
class UInputBufferComponent;
class UMotionWarpingComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageChangedDelegate, float, OldValue, float, NewValue);
DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class APrologueCharacter : public ACharacter, public IAbilitySystemInterface, public IPawnCombatInterface
{
	GENERATED_BODY()

public:
	APrologueCharacter();

	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	
protected:
	virtual void PossessedBy(AController* NewController) override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void OnToughnessTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	virtual void DamageAttributeChanged(const FOnAttributeChangeData& Data);

	void RecoverToughness();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionWarping")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputBufferComponent> InputBufferComponent;
	
	FDelegateHandle ToughnessTagHandle;

	FTimerHandle ToughnessRecoveryTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TSubclassOf<UGameplayEffect> HitReactEffectClass;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "GAS")
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "GAS")
	void InputGAS(const FGameplayTag Tag);
	
	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnDamageChangedDelegate OnDamageChanged;
};

