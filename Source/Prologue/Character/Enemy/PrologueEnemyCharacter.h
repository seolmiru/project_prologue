// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prologue/Pool.h"
#include "Prologue/AbilitySystem/Attribute/PrologueAttributeSet.h"
#include "Prologue/Character/PrologueCharacter.h"
#include "Prologue/UI/PrologueUserWidget.h"
#include "Prologue/Weapon/Projectile/ExplodingMangoProjectile.h"
#include "PrologueEnemyCharacter.generated.h"

class UEnemyWidgetComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedDelegate, float, OldValue, float, NewValue);

USTRUCT(BlueprintType)
struct FWeightedAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AbilityTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.1"))
	float Weight = 1.f;	
};

class UEnemyWidget;
class UGameplayAbility;

UCLASS()
class PROLOGUE_API APrologueEnemyCharacter : public APrologueCharacter
{
	GENERATED_BODY()

public:
	APrologueEnemyCharacter();
	virtual ~APrologueEnemyCharacter() override;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnHealthChangedDelegate OnHealthChanged;

	Pool<AExplodingMangoProjectile>* ProjectilePool;
	
protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	
	void OnDamageAttributeChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StartUpData", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGameplayAbility>> OnGiveAbilities;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StartUpData", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StartUpData", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGameplayEffect>> StartEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	UPrologueAttributeSet* Attributes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEnemyWidgetComponent> HpBar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> BP_EnemyWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEnemyWidget> BossHpBarWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> BP_MangoWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> BP_ChronosWidget;
	
protected:
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool TryActivateRandomAbilityWithWeights(const TArray<FWeightedAbilityInfo>& WeightedAbilities);

	virtual void HealthAttributeChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<AExplodingMangoProjectile> MangoProjectileClass;
	
private:
	FDelegateHandle DamageAttributeChangedHandle;

	bool bHealthBarVisible = false;

	UPROPERTY()
	FGameplayTag LastUsedAbility;
};
