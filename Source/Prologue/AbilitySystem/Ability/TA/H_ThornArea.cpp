// Fill out your copyright notice in the Description page of Project Settings.


#include "H_ThornArea.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
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
}

void AH_ThornArea::SetBoxExtent(const FVector& InBoxExtent)
{
	if (BoxComponent)
	{
		BoxComponent->SetBoxExtent(InBoxExtent, true);
	}
}

void AH_ThornArea::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AH_ThornArea::OnBoxOverlap);

	if (ThornEffect)
	{
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ThornEffect,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector(1.f, 1.f, 1.f),
			true,
			true
		);
	}
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
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(DamageEffectClass, 1.f, ContextHandle);
	if (EffectSpecHandle.IsValid())
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

