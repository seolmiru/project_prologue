// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueBlueprintFunctionLibrary.h"

bool UPrologueBlueprintFunctionLibrary::FindGroundLocation(const UObject* WorldContextObject,
	const FVector& SearchPoint, FVector& OutGroundLocation)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return false;
	}

	const FVector StartLocation = SearchPoint + FVector(0.f, 0.f, 1000.f);
	const FVector EndLocation = SearchPoint - FVector(0.f, 0.f, 1000.f);

	FHitResult HitResult;
	if (World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility))
	{
		OutGroundLocation = HitResult.ImpactPoint;
		return  true;
	}

	return false;
}
