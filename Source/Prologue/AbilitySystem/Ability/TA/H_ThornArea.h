// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "H_ThornArea.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class UBoxComponent;
class UGameplayEffect;

UCLASS()
class PROLOGUE_API AH_ThornArea : public AActor
{
	GENERATED_BODY()

public:
	AH_ThornArea();

	UFUNCTION(BlueprintCallable, Category = "ThornArea")
	void SetBoxExtent(const FVector& InBoxExtent, int32 ThornIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShowDebug = true;
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	TObjectPtr<UNiagaraComponent> ThornNiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> OverlappedActors;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
