// Fill out your copyright notice in the Description page of Project Settings.

#include "ExplodingMangoProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/PrologueGameplayTags.h"

AExplodingMangoProjectile::AExplodingMangoProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	ProjectileCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	ProjectileCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel7, ECR_Block);

	ProjectileMovement->InitialSpeed = 700.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->Velocity = FVector(0.f, 0.f, -1.f);
	ProjectileMovement->ProjectileGravityScale = 1.f;

	ExplosionRadius = 400.f;
	TimeToExplode = 3.f;

	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void AExplodingMangoProjectile::SetPoolRef(Pool<AExplodingMangoProjectile>* PoolRef)
{
	MyPool = PoolRef;
}

void AExplodingMangoProjectile::Active(FVector Location, FRotator Rotation)
{
	SetActorLocation(Location);
	SetActorRotation(Rotation);
	
	ElapsedTime = 0.f;
	bHasExploded = false;

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	ProjectileMovement->SetActive(true);
	ProjectileMovement->Velocity = FVector(0.f, 0.f, -1.f) * ProjectileMovement->InitialSpeed;
	ProjectileMovement->SetUpdatedComponent(ProjectileCollision);

	ProjectileCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	ProjectileCollision->OnComponentHit.RemoveDynamic(this, &ThisClass::OnProjectileHit);
	ProjectileCollision->OnComponentHit.AddDynamic(this, &ThisClass::OnProjectileHit);

	if (ProjectileNiagaraComponent)
	{
		ProjectileNiagaraComponent->Activate(true);
	}

	GetWorldTimerManager().ClearTimer(ExplosionTimerHandle);
}

void AExplodingMangoProjectile::Deactivate()
{
	GetWorldTimerManager().ClearTimer(ExplosionTimerHandle);

	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->SetActive(false);

	ProjectileCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileCollision->OnComponentHit.RemoveDynamic(this, &ThisClass::OnProjectileHit);

	if (ProjectileNiagaraComponent)
	{
		ProjectileNiagaraComponent->Deactivate();
	}

	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	ElapsedTime = 0.f;
	bHasExploded = false;
	TargetActor = nullptr;
}

void AExplodingMangoProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AExplodingMangoProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;

	if (ElapsedTime > TimeToExplode)
	{
		Explode();
		// Destroy();
		// MyPool->Return(this);
		//SetActorTickEnabled(false);
		return;
	}
}

void AExplodingMangoProjectile::OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
                                                UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                                                const FHitResult& Hit)
{
	if (bHasExploded)
	{
		return;
	}
	
	if (OtherComp && OtherComp->GetCollisionObjectType() == ECC_GameTraceChannel7)
	{
		StickAndExplosion(Hit);
	}
}

void AExplodingMangoProjectile::StickAndExplosion(const FHitResult& Hit)
{
	SetActorLocation(Hit.ImpactPoint);
	SetActorRotation(Hit.ImpactNormal.Rotation());

	ProjectileCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement->StopMovementImmediately();
	
	ElapsedTime = 0.f;

	FVector ProjectileLocation = GetActorLocation();

	if (ProjectileEffect)
	{
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ProjectileEffect,
			ProjectileLocation,
			FRotator::ZeroRotator,
			FVector(1.f, 1.f, 1.f),
			true,
			true,
			ENCPoolMethod::AutoRelease
		);
	}

	GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &AExplodingMangoProjectile::Explode, TimeToExplode,
	                                false);
}

void AExplodingMangoProjectile::Explode()
{
	if (bHasExploded)
	{
		return;
	}

	bHasExploded = true;
	
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
			true,
			ENCPoolMethod::AutoRelease
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

	if (!TargetActor)
	{
		// Destroy();
		MyPool->Return(this);
		return;
	}

	const float DistSq = FVector::DistSquared(TargetActor->GetActorLocation(), GetActorLocation());
	if (DistSq > FMath::Square(ExplosionRadius))
	{
		// Destroy();
		MyPool->Return(this);
		return;
	}

	if (TargetActor->Implements<UAbilitySystemInterface>() && DamageEffect)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

		UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetInstigator());

		if (TargetASC && SourceASC)
		{
			FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffect, 1.f, EffectContext);

			if (SpecHandle.IsValid())
			{
				SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, TargetASC);

				FGameplayCueParameters CueParams;
				CueParams.EffectContext = EffectContext;
				TargetASC->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_PlayerHit, CueParams);
			}
		}
	}

	Deactivate();
	// Destroy();
	MyPool->Return(this);
}
