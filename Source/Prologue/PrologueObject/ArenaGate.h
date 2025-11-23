// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ArenaGate.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGateStateChanged, bool, bIsOpen);

UCLASS()
class PROLOGUE_API AArenaGate : public AActor
{
	GENERATED_BODY()

public:
	AArenaGate();

public:
	UPROPERTY(BlueprintAssignable, Category = "Arena")
	FOnGateStateChanged OnGateStateChanged;

	UFUNCTION(BlueprintCallable, Category = "Arena")
	void OpenGate();

	UFUNCTION(BlueprintCallable, Category = "Arena")
	void CloseGate();

	UFUNCTION(BlueprintCallable, Category = "Arena")
	void ActivateArena();

	UFUNCTION(BlueprintCallable, Category = "Arena")
	void CheckEnemies();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate|Effect")
	TObjectPtr<UNiagaraComponent> GateEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate|Effect")
	TObjectPtr<UNiagaraSystem> GateDestroyEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate|Mesh")
	TObjectPtr<UStaticMeshComponent> GateMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate|Trigger")
	TObjectPtr<UBoxComponent> TriggerVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate|Audio")
	TObjectPtr<UAudioComponent> AudioComponent;
	
	UPROPERTY(BlueprintReadWrite, Category = "Arena")
	bool bIsOpen = true;

	UPROPERTY(BlueprintReadWrite, Category = "Arena")
	int32 EnemyCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
	bool bAutoActivate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
	FName EnemyTag = "Enemy";

protected:
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEnemyDestroyed(AActor* DestroyedActor);

private:
	TArray<AActor*> TrackedEnemies;
};
