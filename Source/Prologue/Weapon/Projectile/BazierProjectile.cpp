// Fill out your copyright notice in the Description page of Project Settings.


#include "BazierProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "NiagaraComponent.h"
#include "Containers/Array.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/AbilitySystem/Ability/GA_OverClock.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ABazierProjectile::ABazierProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	
	ProjectileCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	ProjectileCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	ProjectileCollision->OnComponentHit.AddDynamic(this, &ThisClass::OnProjectileHit);

	ProjectileMovement->Deactivate();
	
	ExplosionRadius = 400.f;
	TimeToExplode = 3.f;
	
	bFire = false;
	FireSpeed = 1000.f;
	BazierWeight = 1.f;
	GroundOffset = 100.0f;
}

void ABazierProjectile::FireInDirection(const FVector& ShootDirection)
{
	UE_LOG(LogTemp, Log, TEXT("FireInDirection"));
	FVector MyLocation = GetActorLocation();
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter)
	{
		FVector TargetLocation = PlayerCharacter->GetActorLocation();
		TargetLocation.Z -= GroundOffset;
		SetBazierPoint(MyLocation, TargetLocation);
	}
}

void ABazierProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (UGA_OverClock::bIsOverClockActive)
	{
		CustomTimeDilation = UGA_OverClock::OverClockTimeScale;
	}

	/*if (ProjectileNiagaraComponent)
	{
		ProjectileNiagaraComponent->OnSystemFinished.AddDynamic(this, &ABazierProjectile::OnNiagaraSystemFinished);
	}*/
}

void ABazierProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UE_LOG(LogTemp, Log, TEXT("Tick"));
	if (bFire)
	{
		UE_LOG(LogTemp, Log, TEXT("Fire"));
		// 비행중
		if (CurrentFlyTime < FlyTime)
		{
			FVector CurrentLocation = GetBazierPoint(GetWeight());
			CurrentFlyTime += DeltaTime;
			FVector NextLocation = GetBazierPoint(GetWeight());
			FVector Direction = NextLocation - CurrentLocation;
			Direction.Normalize();

			FRotator LookRotation = Direction.Rotation();
			SetActorRotation(LookRotation);
			SetActorLocation(CurrentLocation);
		}
		// 비행 완료
		else
		{
			ElapsedTime += DeltaTime;

			if (ElapsedTime > TimeToExplode)
			{
				Explode();
				Destroy();
				SetActorTickEnabled(false);
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
	}
}

void ABazierProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherComp && OtherComp->GetCollisionObjectType() == ECC_WorldStatic)
	{
		StickAndExplosion(Hit);
	}
}

void ABazierProjectile::OnNiagaraSystemFinished(UNiagaraComponent* Niagara)
{
	Destroy();
}

void ABazierProjectile::StickAndExplosion(const FHitResult& Hit)
{
	SetActorLocation(Hit.ImpactPoint);
	SetActorRotation(Hit.ImpactNormal.Rotation());

	ProjectileCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bIsStuck = true;
	ElapsedTime = 0.f;
	PrimaryActorTick.bCanEverTick = true;

	GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &ABazierProjectile::Explode, TimeToExplode, false);
}

void ABazierProjectile::Explode()
{
	FVector ExplosionLocation = GetActorLocation();

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

	if (ProjectileExplosion)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ProjectileExplosion,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector(1.f, 1.f, 1.f),
			true,
			true
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
		Destroy();
		return;
	}

	const float DistSq = FVector::DistSquared(TargetActor->GetActorLocation(), GetActorLocation());
	if (DistSq > FMath::Square(ExplosionRadius))
	{
		Destroy();
		return;
	}

	if (TargetActor->Implements<UAbilitySystemInterface>() && DamageEffect)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

		AActor* SourceActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		
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

	Destroy();
}

void ABazierProjectile::SetBazierPoint(FVector MyLocation, FVector TargetLocation)
{
	FVector Direction = TargetLocation - MyLocation;
	Direction.Normalize();

	float Distance = FVector::Dist(MyLocation, TargetLocation);
	float HalfDistance = Distance * 0.5f;

	FVector MiddlePoint = MyLocation + (Direction * HalfDistance);
	MiddlePoint.Z += HalfDistance * Root3 * BazierWeight;

	BazierPoints.Empty();
	BazierPoints.Add(MyLocation);
	BazierPoints.Add(MiddlePoint);
	BazierPoints.Add(TargetLocation);

	float TotalDistance = Distance + (HalfDistance * Root3 * BazierWeight);
	FlyTime = TotalDistance / FireSpeed;
	CurrentFlyTime = 0.0f;

	bFire = true;
}

FVector ABazierProjectile::GetBazierPoint(float weight)
{
	TArray<FVector> DummyPoints = BazierPoints;
	while (DummyPoints.Num() > 1)
	{
		TArray<FVector> CalculatedPoints;
		for (int i = 0; i < DummyPoints.Num() - 1; i++)
		{
			CalculatedPoints.Add(FMath::Lerp(DummyPoints[i], DummyPoints[i + 1], weight));
		}
		DummyPoints = CalculatedPoints;
	}

	return DummyPoints[0];
}
