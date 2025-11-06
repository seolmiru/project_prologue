// Fill out your copyright notice in the Description page of Project Settings.

#include "ChronosScytheProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AChronosScytheProjectile::AChronosScytheProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &AChronosScytheProjectile::OnProjectileBeginOverlap);
	
	ProjectileMovement->InitialSpeed = 1500.f;
	ProjectileMovement->MaxSpeed = 1500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	CurrentState = EProjectileState::MovingForward;
}

void AChronosScytheProjectile::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
}

void AChronosScytheProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == EProjectileState::MovingForward)
	{
		const float DistanceTraveled = FVector::Dist(StartLocation, GetActorLocation());

		if (DistanceTraveled >= MaxTravelDistance)
		{
			CurrentState = EProjectileState::Returning;

			ProjectileMovement->Velocity = -ProjectileMovement->Velocity;
		}
	}
	else
	{
		const FVector DirToStart = (StartLocation - GetActorLocation()).GetSafeNormal();
		const float DotProduct = FVector::DotProduct(DirToStart, ProjectileMovement->Velocity.GetSafeNormal());

		if (DotProduct < 0.f)
		{
			Destroy();
		}
	}
}

void AChronosScytheProjectile::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetInstigator())
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
	}
}

