// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prologue/Character/PrologueCharacter.h"
#include "GameplayTagContainer.h"
#include "Comma.generated.h"

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

protected:
	FVector2D Direction;
	
private:
	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCommaCombatComponent* CommaCombatComponent;

	//Data
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataAsset_InputConfig> InputConfigDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ChracterData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UComboBowData> ComboBowData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ChracterData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UComboSwordData> ComboSwordData;

	//Inputs
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	//GAS
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> SwordWeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BowWeaponMesh;

	void Input_Move(const FInputActionValue& InputActionValue);

	bool HasTag_FocusedAttack() const;

public:
	FORCEINLINE UCommaCombatComponent* GetCommaCombatComponent() const { return CommaCombatComponent; }
	
	FORCEINLINE FVector2D GetDirection() const { return Direction; }
	
	FORCEINLINE UComboBowData* GetComboBowData() const { return ComboBowData; }
	FORCEINLINE UComboSwordData* GetComboSwordData() const { return ComboSwordData; }
	
	FORCEINLINE UAnimMontage* GetBowComboMontage() const { return BowComboMontage; }
	FORCEINLINE UAnimMontage* GetSwordComboMontage() const { return SwordComboMontage; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	UStaticMeshComponent* GetSwordWeaponMesh() const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
	UStaticMeshComponent* GetBowWeaponMesh() const;

	UFUNCTION(BlueprintCallable)
	void RotateToMouse();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> BowComboMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> SwordComboMontage;
};
