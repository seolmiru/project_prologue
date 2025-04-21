// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PrologueFunctionLibrary.generated.h"

class UPrologueAbilitySystemComponent;

UENUM()
enum class EPrologueConfirmType : uint8
{
	Yes,
	No
};

/**
 * 
 */
UCLASS()
class PROLOGUE_API UPrologueFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static UPrologueAbilitySystemComponent* NativeGetPrologueASCFromActor(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "Prologue|FunctionLibrary")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);

	UFUNCTION(BlueprintCallable, Category = "Prologue|FunctionLibrary")
	static void RemoveGameplayFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);

	static bool NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck);

	UFUNCTION(BlueprintCallable, Category = "Prologue|FunctionLibrary", meta = (DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EPrologueConfirmType& OutConfirmType);
};
