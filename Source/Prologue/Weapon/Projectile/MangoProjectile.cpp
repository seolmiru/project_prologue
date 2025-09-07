// Fill out your copyright notice in the Description page of Project Settings.


#include "MangoProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/Prologue.h"
#include "Prologue/PrologueGameplayTags.h"

AMangoProjectile::AMangoProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel7, ECR_Block);
	ProjectileCollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnProjectileHit);

	InitialLifeSpan = 0.f;
	
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->ProjectileGravityScale = 1.f;

		ProjectileMovementComp->InitialSpeed = 100.f;
		ProjectileMovementComp->MaxSpeed = 5000.f;
 
		ProjectileMovementComp->Velocity = FVector(0.f, 0.f, -1.f);
	}

	ExplosionRadius = 300.f;
	TimeToExplode = 3.f;
}

void AMangoProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ElapsedTime += DeltaSeconds;

	if (ElapsedTime > TimeToExplode)
	{
		Explode();
		Destroy();
		return;
	}
}

void AMangoProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	LOG_SCREEN_R("Called OnProjectile Hit");

	if (OtherComp)
	{
		LOG_SCREEN_R("OnProjectileHit Hit Component");

		if (OtherComp && OtherComp->GetCollisionObjectType() == ECC_GameTraceChannel7)
		{
			LOG_SCREEN_R("Check Passed");
			StickAndExplosion(Hit);
		}
		else
		{
			LOG_SCREEN_R("No");

			Destroy();
		}
	}
}

void AMangoProjectile::StickAndExplosion(const FHitResult& Hit)
{
	SetActorLocation(Hit.ImpactPoint);
	SetActorRotation(Hit.ImpactNormal.Rotation());
	
	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ElapsedTime = 0.f;
	
	GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &AMangoProjectile::Explode, TimeToExplode, false);
}

void AMangoProjectile::Explode()
{
	FVector ExplosionLocation = GetActorLocation();

	if (ExplosionEffect)
	{
		UNiagaraComponent* ExplosionComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionEffect,
			ExplosionLocation,
			FRotator::ZeroRotator,
			FVector(1.f, 1.f, 1.f),
			true,
			true
		);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			ExplosionLocation,
			1.f,
			1.f
		);
	}

	if (bShowDebug)
	{
		DrawDebugSphere(
			GetWorld(),
			GetActorLocation(),
			ExplosionRadius,
			32,
			FColor::Red,
			false,
			2.f,
			0.f,
			2.f
		);
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn == nullptr)
	{
		Destroy();
		return;
	}

	const float DistSq = FVector::DistSquared(PlayerPawn->GetActorLocation(), GetActorLocation());
	if (DistSq > FMath::Square(ExplosionRadius))
	{
		Destroy();
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

			FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(AttackDamageEffect, 1.f, EffectContext);

			if (SpecHandle.IsValid())
			{
				SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, TargetASC);

				FGameplayCueParameters CueParams;
				CueParams.EffectContext = EffectContext;
				TargetASC->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_PlayerHit, CueParams);
			}
		}
	}
	
	Destroy();
}
