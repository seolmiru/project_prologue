// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chronoslino.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class UGameplayEffect;

UCLASS()
class PROLOGUE_API AChronoslino : public AActor
{
	GENERATED_BODY()

public:
	AChronoslino();

	UFUNCTION(BlueprintCallable)
	void StartLino(float Delay, float InInnerRadius, float InOuterRadius);

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lino")
	float InnerRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lino")
	float OuterRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lino")
	float TimeToExplode = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lino")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesToQuery;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lino|GAS")
	TSubclassOf<UGameplayEffect> LinoDamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lino")
	TObjectPtr<UNiagaraComponent> LinoStartNiagara;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lino")
	TObjectPtr<UNiagaraSystem> LinoExplodeNiagara;
	
private:
	FTimerHandle LinoTimerHandle;

	void PerformLinoCheck();
};
