// Fill out your copyright notice in the Description page of Project Settings.


#include "ChronosProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AChronosProjectile::AChronosProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ProjectileCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	ProjectileCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	//ProjectileCollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnProjectileHit);
	ProjectileCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnProjectileBeginOverlap);

	ProjectileMovement->InitialSpeed = 700.f;
	ProjectileMovement->MaxSpeed = 900.f;
	ProjectileMovement->Velocity = FVector(1.f, 0.f, 0.f);
	ProjectileMovement->ProjectileGravityScale = 0.f;

	InitialLifeSpan = 4.f;
}

void AChronosProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AChronosProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 벽이나 바닥에 부딪혔을 때만 파괴
	if (OtherActor && !OtherActor->GetClass()->IsChildOf(APawn::StaticClass()))
	{
		Destroy();
	}
}

void AChronosProjectile::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || !DamageEffect)
	{
		return;
	}

	// 투사체를 발사한 Actor 충돌 무시
	if (OtherActor == GetInstigator())
	{
		return;
	}

	// 투사체끼리의 충돌 무시
	if (Cast<AChronosProjectile>(OtherActor))
	{
		return;
	}
	
	if (OtherActor->Implements<UAbilitySystemInterface>())
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
		UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetInstigator());

		if (TargetASC && SourceASC)
		{
			FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);
			EffectContext.AddHitResult(SweepResult);

			// 체력과 강인도를 감소시키는 Effect 적용
			FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffect, 1.f, EffectContext);
			
			if (SpecHandle.IsValid())
			{
				SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, TargetASC);
				FGameplayCueParameters CueParams;
				CueParams.EffectContext = EffectContext;
				//TargetASC->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_EnemyHit, CueParams);
			}
		}

		Destroy();
	}
}

