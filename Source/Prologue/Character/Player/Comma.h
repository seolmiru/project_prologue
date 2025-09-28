// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prologue/Character/PrologueCharacter.h"
#include "GameplayTagContainer.h"
#include "PlayerDashPoint.h"
#include "Prologue/Pool.h"
#include "Comma.generated.h"

class AShopKeeper;
class UNiagaraComponent;
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
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> DashCollision;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraComponent> SwordAuraEffect;*/
	
	/** Data */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataAsset_InputConfig> InputConfigDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ChracterData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UComboSwordData> ComboSwordData;
	
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ChracterData", meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UComboBowData> ComboBowData;
	
	/** Inputs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction = nullptr;

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
	TObjectPtr<USceneComponent> UIAnchorComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> SmashAttackWidgetComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> BP_SmashAttackWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> OverClockWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> CooldownWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> BP_CooldownWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> GuideWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> BP_GuideWidget;

	void Input_Move(const FInputActionValue& InputActionValue);

	/** Camera Settings Function */
protected:
	UFUNCTION(BlueprintCallable, Category = "Camera|Rotate")
	void ActivateRotateCamera(FRotator NewTargetRotation);

	UFUNCTION(BlueprintCallable, Category = "Camera|Rotate")
	void DeactivateRotateCamera();

	UFUNCTION(BlueprintCallable, Category = "Camera|Length")
	void ActivateAdjustCamera(float NewTargetArmLength);

	UFUNCTION(BlueprintCallable, Category = "Camera|Length")
	void DeactivateAdjustCamera();
	
	/** Camera Settings Variables */
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	FRotator DefaultCameraRelativeRotation = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	FRotator TargetCameraRelativeRotation = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CameraRotationInterpolationSpeed = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float DefaultCameraArmLength = 1600.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float TargetCameraArmLength = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CameraArmLengthInterpolationSpeed = 2.f;

	/** Effect Material Settings */
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

	FORCEINLINE UAnimMontage* GetSwordSmashAttackMontage() const { return SwordSmashAttackMontage; }
	//FORCEINLINE UAnimMontage* GetBowSwitchAttackMontage() const { return BowSwitchAttackMontage; }

	FVector2D GetCachedMovementInput() const { return CachedMovementInput; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	UStaticMeshComponent* GetSwordWeaponMesh() const;

	FORCEINLINE UCommaWidget* GetCommaWidget() const { return CommaWidget; }
	FORCEINLINE UWidgetComponent* GetCooldownWidget() const { return CooldownWidgetComponent; }

	FORCEINLINE UCapsuleComponent* GetDashCollision() const { return DashCollision; }

	FORCEINLINE UWidgetComponent* GetGuideWidget() const { return GuideWidgetComponent; }

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

	void OnSmashAttackUI(const FGameplayTag CallbackTag, int32 NewCount) const;

	UFUNCTION(BlueprintCallable, Category = "VFX")
	void TriggerDamageEffect(float DamageAmount = 1.f);

	/** Speed Boost */
protected:
	UFUNCTION()
	void OnDashSpeedBoost(const FGameplayTag CallbackTag, int32 NewCount);

	float DefaultWalkSpeed;

	FGameplayTag IsMovingTag;
	FGameplayTag SpeedBoostTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed Boost")
	float SpeedBoost = 1.5f;

	/** Shop */
public:
	UFUNCTION(BlueprintCallable)
	void OnInteractShop();
	
	UFUNCTION(BlueprintCallable)
	void OnInteractShopCompleted();

	void PurchaseHealPotion();

	FTimerHandle PurchaseTimerHandle;

	UPROPERTY()
	TObjectPtr<AShopKeeper> ShopKeeperInRange;

public:
	void SetShopKeeper(AShopKeeper* ShopKeeper);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> SwordComboMontage;
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	//TObjectPtr<class UAnimMontage> BowComboMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> SwordSmashAttackMontage;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	//TObjectPtr<class UAnimMontage> BowSwitchAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag SmashAttackSwordTag;

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

	AActor* GetGround() const;
	
	void InputDash(bool bInput);

	bool GetInputDashState() const;	
private:
	TObjectPtr<class APlayerDashPoint> DashPoint;

	UPROPERTY(VisibleAnywhere, Category="Input")
	bool bInputDash;

	// class Pool<class APlayerDashPoint> TestPool;
};