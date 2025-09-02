// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerBank.generated.h"

class UTimelineComponent;
class UBoxComponent;

UCLASS()
class PROLOGUE_API APowerBank : public AActor
{
	GENERATED_BODY()

public:
	APowerBank();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	TObjectPtr<UBoxComponent> TriggerVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	TObjectPtr<UStaticMeshComponent> PowerBankMesh;

	UPROPERTY()
	TObjectPtr<UTimelineComponent> MaterialTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	UCurveFloat* TimelineCurve;
	
	UPROPERTY(EditAnywhere, Category = "Data")
	FName PowerBankID;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsInteracted = false;
	
protected:
	UFUNCTION(BlueprintCallable)
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void Interact();

	UFUNCTION()
	void TimelineProgress(float Value);
};
