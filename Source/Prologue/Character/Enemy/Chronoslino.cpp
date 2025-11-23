// Fill out your copyright notice in the Description page of Project Settings.


#include "Chronoslino.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/OverlapResult.h"
#include "Prologue/Prologue.h"

AChronoslino::AChronoslino()
{
	PrimaryActorTick.bCanEverTick = false;
	InnerRadius = 500.f;
	OuterRadius = 1000.f;

	LinoStartNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LinoStartNiagara"));
}

void AChronoslino::StartLino(float Delay, float InInnerRadius, float InOuterRadius)
{
	InnerRadius = InInnerRadius;
	OuterRadius = InOuterRadius;
	TimeToExplode = Delay;

	GetWorld()->GetTimerManager().ClearTimer(LinoTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(LinoTimerHandle, this, &AChronoslino::PerformLinoCheck, Delay, false);

	DrawDebugSphere(
		GetWorld(),
		GetActorLocation(),
		OuterRadius,
		32,
		FColor::Green,
		false,
		Delay
		);

	DrawDebugSphere(
		GetWorld(),
		GetActorLocation(),
		InnerRadius,
		32,
		FColor::Red,
		false,
		Delay
		);
}

void AChronoslino::BeginPlay()
{
	Super::BeginPlay();

	StartLino(TimeToExplode, InnerRadius, OuterRadius);
}

void AChronoslino::PerformLinoCheck()
{
	if (LinoExplodeNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LinoExplodeNiagara,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector(1.f, 1.f, 1.f),
			true,
			true
		);
	}
	
	TArray<FOverlapResult> OutOverlaps;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetInstigator())
	{
		QueryParams.AddIgnoredActor(GetInstigator());
	}
	
	const FVector CenterLocation = GetActorLocation();
	
	bool bHasOverlaps = GetWorld()->OverlapMultiByObjectType(
		OutOverlaps,
		CenterLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams(ObjectTypesToQuery),
		FCollisionShape::MakeSphere(OuterRadius),
		QueryParams
	);

	if (bHasOverlaps)
	{
		TSet<AActor*> ActorsInLino;

		for (const FOverlapResult& Result : OutOverlaps)
		{
			AActor* HitActor = Result.GetActor();
			if (!HitActor || ActorsInLino.Contains(HitActor))
			{
				continue;
			}

			const float DistSquared = FVector::DistSquared(CenterLocation, HitActor->GetActorLocation());
			const float InnerRadiusSquared = InnerRadius * InnerRadius;

			if (DistSquared > InnerRadiusSquared)
			{
				ActorsInLino.Add(HitActor);
			}
		}
		
		for (AActor* TargetActor : ActorsInLino)
		{
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

			if (!TargetASC || !LinoDamageEffect)
			{
				continue;
			}

			FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
			ContextHandle.AddSourceObject(this);

			const FGameplayEffectSpecHandle DamageEffectSpecHandle = TargetASC->MakeOutgoingSpec(LinoDamageEffect, 1.f, ContextHandle);

			if (DamageEffectSpecHandle.IsValid())
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
				LOG_SCREEN_R("Hit Lino");
			}
		}
	}

	Destroy();
}

