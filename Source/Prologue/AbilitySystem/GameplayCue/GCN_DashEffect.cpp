// Fill out your copyright notice in the Description page of Project Settings.


#include "Prologue/AbilitySystem/GameplayCue/GCN_DashEffect.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"

AGCN_DashEffect::AGCN_DashEffect()
{
	bAutoDestroyOnRemove = false;
}

bool AGCN_DashEffect::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	Super::OnActive_Implementation(MyTarget, Parameters);

	if (IsValid(ActiveEffectComponent))
	{
		ActiveEffectComponent->DestroyComponent();
		ActiveEffectComponent = nullptr;
	}

	if (DashEffect && IsValid(MyTarget))
	{
		ACharacter* TargetCharacter = Cast<ACharacter>(MyTarget);
		if (TargetCharacter && TargetCharacter->GetMesh())
		{
			ActiveEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
				DashEffect,
				TargetCharacter->GetMesh(),
				NAME_None,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::KeepRelativeOffset,
				true
			);
		}
	}

	return true;
}

bool AGCN_DashEffect::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	Super::OnRemove_Implementation(MyTarget, Parameters);

	if (IsValid(ActiveEffectComponent))
	{
		ActiveEffectComponent->DestroyComponent();
		ActiveEffectComponent = nullptr;
	}

	return true;
}
