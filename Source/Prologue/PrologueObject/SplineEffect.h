#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Curves/CurveFloat.h"
#include "SplineEffect.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USplineComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(OnStartSplineEffect, AActor*, FVector)
DECLARE_MULTICAST_DELEGATE_TwoParams(OnEndSplineEffecct, AActor*, FVector)

UCLASS()
class PROLOGUE_API ASplineEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	ASplineEffect();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	void OnStartSpline(FVector StartPosition, AActor* targetActor,
		OnStartSplineEffect StartEventCallbackHandler,
		OnEndSplineEffecct EndEventCallbackHandler,
		float PeakHeight = 300.0f/*곡선 최고점 높이*/);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UNiagaraSystem> SoulNiagaraTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UNiagaraSystem> GetSoulEffectTemplate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<UNiagaraComponent> SoulEffectComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveSpeed")
	float Duration = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PeakHeight")
	float ArcHeight = 300.f; // 포물선 최고점 높이 (Z 오프셋)
private:
	float ElapsedTime;
	FVector StartLocation;
	float DistanceAlongSpline;

	OnStartSplineEffect startEventHandler;
	OnEndSplineEffecct endEventHandler;
};
