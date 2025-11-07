// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShopKeeper.generated.h"

class UWidgetComponent;
class UCapsuleComponent;
class UGameplayEffect;
class UBoxComponent;

UCLASS()
class PROLOGUE_API AShopKeeper : public AActor
{
	GENERATED_BODY()

public:
	AShopKeeper();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ShopKeeper|Cost")
	float HealPotionCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ShopKeeper|Effect")
	TSubclassOf<UGameplayEffect> CostEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ShopKeeper|Effect")
	TSubclassOf<UGameplayEffect> RewardEffect;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShopKeeper")
	TObjectPtr<UBoxComponent> TriggerVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShopKeeper")
	TObjectPtr<USkeletalMeshComponent> Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShopKeeper")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShopKeeper")
	TObjectPtr<UWidgetComponent> ShopKeeperWidgetComponent;
	
protected:
	UFUNCTION(BlueprintCallable)
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
