// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prologue/Character/PrologueCharacter.h"
#include "GameplayTagContainer.h"
#include "Comma.generated.h"

class UCommaWidget;
class UComboSwordData;
class UComboBowData;
class UInputAction;
class UInputMappingContext;
class UCommaCombatComponent;
class USpringArmComponent;
class UCameraComponent;
class UDataAsset_InputConfig;
struct FInputActionValue;

UCLASS()
class PROLOGUE_API AComma : public APrologueCharacter
{
	GENERATED_BODY()

public:
	AComma();

	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void NotifyControllerChanged() override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
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
	UCommaCombatComponent* CommaCombatComponent;

	/** Data */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataAsset_InputConfig> InputConfigDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ChracterData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UComboSwordData> ComboSwordData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ChracterData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UComboBowData> ComboBowData;
	
	/** Inputs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** GAS */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StartUpData", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StartUpData", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGameplayEffect>> StartEffect;

	/** Mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> SwordWeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BowWeaponMesh;

	/** UI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> BP_CommaWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCommaWidget> CommaWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* UIAnchorComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* SwitchAttackWidgetComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> BP_SwitchAttackWidget;
	
	void Input_Move(const FInputActionValue& InputActionValue);
	
public:
	FORCEINLINE UCommaCombatComponent* GetCommaCombatComponent() const { return CommaCombatComponent; }
	
	FORCEINLINE FVector2D GetDirection() const { return Direction; }
	
	FORCEINLINE UComboSwordData* GetComboSwordData() const { return ComboSwordData; }
	FORCEINLINE UComboBowData* GetComboBowData() const { return ComboBowData; }
	
	FORCEINLINE UAnimMontage* GetSwordComboMontage() const { return SwordComboMontage; }
	FORCEINLINE UAnimMontage* GetBowComboMontage() const { return BowComboMontage; }

	FORCEINLINE UAnimMontage* GetSwordSwitchAttackMontage() const { return SwordSwitchAttackMontage; }
	FORCEINLINE UAnimMontage* GetBowSwitchAttackMontage() const { return BowSwitchAttackMontage; }

	FVector2D GetCachedMovementInput() const { return CachedMovementInput; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	UStaticMeshComponent* GetSwordWeaponMesh() const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
	UStaticMeshComponent* GetBowWeaponMesh() const;

	void RotateToMouse();

	void RotateToMouseSmooth();
	
	void RotateToTarget(AActor* Target);

	void OnAttackEnded();

	void OnSwitchAttackUI(const FGameplayTag CallbackTag, int32 NewCount) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> SwordComboMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> BowComboMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> SwordSwitchAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> BowSwitchAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag SwitchAttackSwordTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag SwitchAttackBowTag;
	
private:
	FRotator TargetRotation = FRotator::ZeroRotator;
	float RotationInterpSpeed = 12.f;
	bool bIsUsingSmoothRotation = false;
};
