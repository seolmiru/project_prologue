// Fill out your copyright notice in the Description page of Project Settings.


#include "H_ThornArea.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AH_ThornArea::AH_ThornArea()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	ThornNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ThornNiagaraComponent"));
	ThornNiagaraComponent->SetupAttachment(RootComponent);
}

void AH_ThornArea::SetBoxExtent(const FVector& InBoxExtent, int32 ThornIndex)
{
	if (BoxComponent)
	{
		BoxComponent->SetBoxExtent(InBoxExtent, true);
	}

	if (ThornNiagaraComponent)
	{
		const float NewSpawnCount = 50.f + (ThornIndex * 25.f);

		const FVector NiagaraBoxSize = InBoxExtent * 2.f;
		
		ThornNiagaraComponent->SetFloatParameter(TEXT("Count"), NewSpawnCount);
		
		ThornNiagaraComponent->SetVectorParameter(TEXT("SpawnThornAreaExtent"), NiagaraBoxSize);
	}
}

void AH_ThornArea::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AH_ThornArea::OnBoxOverlap);
}

void AH_ThornArea::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bShowDebug)
	{
		UKismetSystemLibrary::DrawDebugBox(
			GetWorld(),
			BoxComponent->GetComponentLocation(),
			BoxComponent->GetScaledBoxExtent(),
			FLinearColor::Red,
			BoxComponent->GetComponentRotation(),
			3.f,
			2.f
		);
	}
}

void AH_ThornArea::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetInstigator() || !DamageEffectClass || OverlappedActors.Contains(OtherActor))
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (!TargetASC)
	{
		return;
	}

	OverlappedActors.Add(OtherActor);

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

