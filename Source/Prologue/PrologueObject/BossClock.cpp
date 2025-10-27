// Fill out your copyright notice in the Description page of Project Settings.


#include "BossClock.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"

ABossClock::ABossClock()
{
	PrimaryActorTick.bCanEverTick = true;

	ClockCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ClockCollision"));
	SetRootComponent(ClockCollision);
	ClockCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ClockCollision->SetCollisionResponseToAllChannels(ECR_Ignore);

	ClockHandNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ClockHandNiagaraComponent"));
	ClockHandNiagaraComponent->SetupAttachment(RootComponent);
}

void ABossClock::BeginPlay()
{
	Super::BeginPlay();

	ClockCollision->OnComponentBeginOverlap.AddDynamic(this, &ABossClock::OnBoxOverlap);
}

void ABossClock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float YawValue = ClockSpeed * DeltaTime;

	const FRotator DeltaRotation = FRotator(0.f, YawValue, 0.f);

	AddActorLocalRotation(DeltaRotation);
}

void ABossClock::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetInstigator() || !DamageEffectClass)
	{
		return;
	}
 	
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (!TargetASC) return;

	FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	if (DamageEffectClass)
	{
		
		const FGameplayEffectSpecHandle DamageEffectSpecHandle = TargetASC->MakeOutgoingSpec(DamageEffectClass, 1.f, ContextHandle);
		if (DamageEffectSpecHandle.IsValid())
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
	}

	if (ImmunityEffectClass)
	{
		const FGameplayEffectSpecHandle ImmunityEffectSpecHandle = TargetASC->MakeOutgoingSpec(ImmunityEffectClass, 1.f, ContextHandle);
		if (ImmunityEffectSpecHandle.IsValid())
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*ImmunityEffectSpecHandle.Data.Get());
		}
	}
}

