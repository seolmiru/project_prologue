// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PrologueFunctionLibrary.generated.h"

UENUM()
enum class EConfirmType : uint8
{
	Yes,
	No
};

class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class PROLOGUE_API UPrologueFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static UAbilitySystemComponent* NativeGetASCFromActor(AActor* InActor);
	
	UFUNCTION(BlueprintCallable, Category = "Prologue|FunctionLibrary")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);
	
	UFUNCTION(BlueprintCallable, Category = "Prologue|FunctionLibrary")
	static void RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);

	UFUNCTION(BlueprintCallable, Category = "Prologue|FunctionLibrary")
	static bool IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn);

	static bool NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck);

	UFUNCTION(BlueprintCallable, Category = "Prologue|FunctionLibrary", meta = (DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EConfirmType& OutConfirmType);

	UFUNCTION(BlueprintCallable, Category = "AI|Platform")
	static AActor* GetCurrentStandingPlatform(AActor* TargetActor, float TraceDistance = 500.f);
};
