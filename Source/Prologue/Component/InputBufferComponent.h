// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "InputBufferComponent.generated.h"

USTRUCT(BlueprintType)
struct FBufferedInput
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag InputTag;

	UPROPERTY()
	float Timestamp;

	FBufferedInput()
	{
		Timestamp = 0.f;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROLOGUE_API UInputBufferComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInputBufferComponent();

	UFUNCTION(BlueprintCallable, Category = "Input Buffer")
	void BufferInput(FGameplayTag InputTag);

	UFUNCTION(BlueprintCallable, Category = "Input Buffer")
	bool ConsumeBufferedInput();
	
	UFUNCTION(BlueprintCallable, Category = "Input Buffer")
	bool HasBufferedInput(FGameplayTag InputTag) const;

	UFUNCTION(BlueprintCallable, Category = "Input Buffer")
	void ClearBuffer();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	TArray<FBufferedInput> InputBuffer;

	UPROPERTY(EditDefaultsOnly, Category = "Input Buffer")
	int32 MaxBufferSize = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Input Buffer")
	float InputLifeTime = 0.3f;

	void CleanExpiredInputs();

	void ExecuteInput(FGameplayTag InputTag);
};
