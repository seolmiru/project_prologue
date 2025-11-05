// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "Prologue/UI/Object/PowerBankIconWidget.h"
#include "PowerBank.generated.h"

class UNiagaraComponent;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> PowerBankMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	TObjectPtr<UNiagaraComponent> ActivateNiagara;
 	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanInteracted = false;

	/* Start Sejin */

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	// TObjectPtr<UWidgetComponent> WidgetComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "UI")
	TObjectPtr<USceneComponent> AttachPoint;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "UI")
	TSubclassOf<UPowerBankIconWidget> BP_IconWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UPowerBankIconWidget> IconWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<USoundBase> ActivateSound;
	
	/* End Sejin */
protected:
	UFUNCTION(BlueprintCallable)
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION(BlueprintCallable)
	void Interact();

	UFUNCTION()
	void TimelineProgress(float Value);

	/* Start Sejin */

	UFUNCTION()
	void AttachWidget();

	
	
	/* End Sejin */
};