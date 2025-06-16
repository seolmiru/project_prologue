// Fill out your copyright notice in the Description page of Project Settings.


#include "MangoProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/PrologueGameplayTags.h"

void AMangoProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 벽이나 바닥에 부딪혔을 때만 파괴
	if (OtherActor && !OtherActor->GetClass()->IsChildOf(APawn::StaticClass()))
	{
		Destroy();
	}
}

void AMangoProjectile::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 투사체를 발사한 Actor 충돌 무시
	if (OtherActor == GetInstigator())
	{
		return;
	}

	// 투사체끼리의 충돌 무시
	if (Cast<APrologueProjectileBase>(OtherActor))
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn == nullptr)
	{
		return;
	}
	
	if (PlayerPawn->Implements<UAbilitySystemInterface>() && AttackDamageEffect)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PlayerPawn);
		UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetInstigator());

		if (TargetASC && SourceASC)
		{
			FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);
			EffectContext.AddHitResult(SweepResult);

			// 체력과 강인도를 감소시키는 Effect 적용
			FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(AttackDamageEffect, 1.f, EffectContext);
			FGameplayEffectSpecHandle HitReactSpecHandle = SourceASC->MakeOutgoingSpec(ToughnessDamageEffect, 1.f, EffectContext);
			
			if (SpecHandle.IsValid())
			{
				SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, TargetASC);
				SourceASC->ApplyGameplayEffectSpecToTarget(*HitReactSpecHandle.Data, TargetASC);

				FGameplayCueParameters CueParams;
				CueParams.EffectContext = EffectContext;
				TargetASC->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_PlayerHit, CueParams);
			}
		}
	}
}