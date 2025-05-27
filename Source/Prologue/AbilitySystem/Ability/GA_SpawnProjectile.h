// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_SpawnProjectile.generated.h"

UENUM(BlueprintType)
enum class ELaunchDirectionType : uint8
{
	Forward UMETA(DisplayName = "Forward"),
	CustomLocalDirection UMETA(DisplayName = "Custom Local Direction"),
	FindTargetProjectile UMETA(DisplayName = "Find Target Projectile"),
};

/**
 * 
 */
UCLASS()
class PROLOGUE_API UGA_SpawnProjectile : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_SpawnProjectile();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<AActor> PerfectProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	FGameplayTag PerfectShotRequiredTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	ELaunchDirectionType LaunchDirectionType = ELaunchDirectionType::Forward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Custom")
	FVector CustomLaunchDirection = FVector(0.f, 0.f, -1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Custom")
	bool bFindTarget = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Custom")
	float TargetSearchRadius = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Custom")
	float TargetSearchAngle = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Custom")
	int32 NumProjectiles = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Custom")
	float SpreadHalfAngle = 0.f;

private:
	AActor* FindTarget(AActor* AvatarActor, FVector& OutTargetLocation) const;
};
