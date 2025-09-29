// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProgressBar.h"
#include "Prologue/AbilitySystem/Attribute/PrologueAttributeSet.h"
#include "Prologue/UI/PrologueUserWidget.h"
#include "EnemyWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROLOGUE_API UEnemyWidget : public UPrologueUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;

protected:
	virtual void OnHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* CurrentHealthBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* DelayedHealthBar;

	// 체력바 설정값
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	float CurrentHealth = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	float CurrentMaxHealth = 0.1f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	float DelayedHealth = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float HealthPercent = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float DelayedHealthPercent = 0.f;

	bool bIsDelayedHealth = false;

	// 체력바 감소 애니메이션 설정값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float DelayAnimation = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float AnimationSpeed = 2.f;
	
	FTimerHandle DelayedHealthTimerHandle;
	FTimerHandle HealthAnimationTimerHandle;
};
