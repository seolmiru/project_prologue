// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplodingProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/AbilitySystem/Ability/GA_OverClock.h"

AExplodingProjectile::AExplodingProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollisionBox"));
	SetRootComponent(ProjectileCollisionBox);
	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	ProjectileCollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnProjectileHit);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->UpdatedComponent = ProjectileCollisionBox;
	ProjectileMovementComp->InitialSpeed = 1200.f;
	ProjectileMovementComp->MaxSpeed = 1200.f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->ProjectileGravityScale = 1.f;

	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());

	ExplosionRadius = 400.f;
	TimeToExplode = 3.f;
}

void AExplodingProjectile::FireInDirection(const FVector& ShootDirection) const
{
	ProjectileMovementComp->Velocity = ShootDirection * ProjectileMovementComp->InitialSpeed;
}

void AExplodingProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (UGA_OverClock::bIsOverClockActive)
	{
		CustomTimeDilation = UGA_OverClock::OverClockTimeScale;
	}
}

void AExplodingProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bIsStuck)
		return;

	ElapsedTime += DeltaSeconds;

	if (ElapsedTime > TimeToExplode)
	{
		Explode();
		Destroy();
		return;
	}

	if (UGA_OverClock::bIsOverClockActive)
	{
		if (CustomTimeDilation != UGA_OverClock::OverClockTimeScale)
			CustomTimeDilation = UGA_OverClock::OverClockTimeScale;
	}
	else if (CustomTimeDilation != 1.f)
	{
		CustomTimeDilation = 1.f;
	}
}

void AExplodingProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherComp && OtherComp->GetCollisionObjectType() == ECC_WorldStatic)
	{
		StickAndExplosion(Hit);
	}
}

void AExplodingProjectile::StickAndExplosion(const FHitResult& Hit)
{
	ProjectileMovementComp->StopMovementImmediately();
	ProjectileMovementComp->Deactivate();

	SetActorLocation(Hit.ImpactPoint);
	SetActorRotation(Hit.ImpactNormal.Rotation());

	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bIsStuck = true;
	ElapsedTime = 0.f;
	PrimaryActorTick.bCanEverTick = true;
	
	GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &AExplodingProjectile::Explode, TimeToExplode, false);
}

void AExplodingProjectile::Explode()
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

