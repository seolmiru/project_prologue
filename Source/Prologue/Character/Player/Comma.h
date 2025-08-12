// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prologue/Character/PrologueCharacter.h"
#include "GameplayTagContainer.h"
#include "Comma.generated.h"

class UWidgetComponent;
class UPostProcessComponent;
class UCommaWidget;
class UComboSwordData;
class UInputAction;
class UInputMappingContext;
class UCommaCombatComponent;
class USpringArmComponent;
class UCameraComponent;
class UDataAsset_InputConfig;
class UFallPreventionComponent;
struct FInputActionValue;

UCLASS()
class PROLOGUE_API AComma : public APrologueCharacter
{
	GENERATED_BODY()

public:
	AComma();

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void NotifyControllerChanged() override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadOnly, Category = "Combo")
	int32 CurrentSwordCombo = 0;
	
protected:
	virtual void PostInitializeComponents() override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	FVector2D Direction;

	UPROPERTY()
	FVector2D CachedMovementInput = FVector2D::ZeroVector;
	
private:
	/** Components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* ParryCollision;
	
	/** Data */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataAsset_InputConfig> InputConfigDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ChracterData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UComboSwordData> ComboSwordData;
	
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ChracterData", meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UComboBowData> ComboBowData;
	
	/** Inputs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** GAS */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StartUpData", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGameplayAbility>> OnGiveAbilities;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StartUpData", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StartUpData", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGameplayEffect>> StartEffect;

	/** Mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> SwordWeaponMesh;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UStaticMeshComponent> BowWeaponMesh;

	/** UI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> BP_CommaWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCommaWidget> CommaWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* UIAnchorComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> SwitchAttackWidgetComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> BP_SwitchAttackWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> OverClockWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> CooldownWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> BP_CooldownWidget;
	
	void Input_Move(const FInputActionValue& InputActionValue);

private:
	/** Camera Settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float DefaultZoomDist = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float IntroZoomDist = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float ZoomOutInterpSpeed = 4.f;
	
	float TargetZoomDist = 1200.f;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMaterial> DamagePostProcessMaterial;
    
	UPROPERTY()
	TObjectPtr<class UMaterialInstanceDynamic> DamagePostProcessMID;
    
	UPROPERTY()
	FTimerHandle DamageEffectTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX", meta = (AllowPrivateAccess = "true"))
	float DamageEffectStartTime = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX", meta = (AllowPrivateAccess = "true"))
	float DamageEffectDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX", meta = (AllowPrivateAccess = "true"))
	float DamageEffectIntensity = 1.f;
	
public:
	FORCEINLINE FVector2D GetDirection() const { return Direction; }
	
	FORCEINLINE UComboSwordData* GetComboSwordData() const { return ComboSwordData; }
	//FORCEINLINE UComboBowData* GetComboBowData() const { return ComboBowData; }
	
	FORCEINLINE UAnimMontage* GetSwordComboMontage() const { return SwordComboMontage; }
	//FORCEINLINE UAnimMontage* GetBowComboMontage() const { return BowComboMontage; }

	FORCEINLINE UAnimMontage* GetSwordSwitchAttackMontage() const { return SwordSwitchAttackMontage; }
	//FORCEINLINE UAnimMontage* GetBowSwitchAttackMontage() const { return BowSwitchAttackMontage; }

	FVector2D GetCachedMovementInput() const { return CachedMovementInput; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	UStaticMeshComponent* GetSwordWeaponMesh() const;

	FORCEINLINE UCapsuleComponent* GetParryCollision() const { return ParryCollision; }

	FORCEINLINE UCommaWidget* GetCommaWidget() const { return CommaWidget; }
	FORCEINLINE UWidgetComponent* GetCooldownWidget() const { return CooldownWidgetComponent; }

	void SetUIVisibility(bool bVisible);

	void HideCommaUI();

	void ShowCommaUI();
	
	//UFUNCTION(BlueprintPure, Category = "Weapon")
	//UStaticMeshComponent* GetBowWeaponMesh() const;

	void RotateToMouse();

	void RotateToMouseSmooth();
	
	void RotateToTarget(AActor* Target);

	FVector GetMouseDirection() const;

	void OnAttackEnded();

	void OnSwitchAttackUI(const FGameplayTag CallbackTag, int32 NewCount) const;

	UFUNCTION(BlueprintCallable, Category = "VFX")
	void TriggerDamageEffect(float DamageAmount = 1.f);

	void ZoomIn(float ZoomDist = 600.f);

	void ZoomOut();

	void ResetZoom();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> SwordComboMontage;
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	//TObjectPtr<class UAnimMontage> BowComboMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> SwordSwitchAttackMontage;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	//TObjectPtr<class UAnimMontage> BowSwitchAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag SwitchAttackSwordTag;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
	//FGameplayTag SwitchAttackBowTag;
	
private:
	FRotator TargetRotation = FRotator::ZeroRotator;
	float RotationInterpSpeed = 12.f;
	bool bIsUsingSmoothRotation = false;

	bool bCommaWidgetVisibility = true;
	bool bCooldownWidgetVisibility = true;

	void UpdateDamageEffect();

	/** Sejin */
public:
	class APlayerDashPoint* GetDashPoint() const;
	
	TObjectPtr<class APlayerDashPoint> DashPoint;
};