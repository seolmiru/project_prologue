// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_WeaponTrace.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponTraceResultDelegate, const FHitResult&, HitResult);

/**
 * 
 */
UCLASS()
class PROLOGUE_API UAT_WeaponTrace : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAT_WeaponTrace();

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "WeaponTrace", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAT_WeaponTrace* CreateWeaponTraceTask(UGameplayAbility* OwningAbility, FName TaskInstanceName, UStaticMeshComponent* WeaponMesh, FName StartSocket, FName EndSocket);

	UPROPERTY(BlueprintAssignable)
	FWeaponTraceResultDelegate OnHit;

protected:
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
	float TraceSphereRadius = 80.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace|Debug")
	bool bShowDebug = true;
	
private:
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	FName StartSocket;
	FName EndSocket;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> HitActors;

	FVector LastStartSocketLocation;
};
