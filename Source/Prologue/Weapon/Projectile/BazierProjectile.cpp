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

// Sets default values
ABazierProjectile::ABazierProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollisionBox"));
	SetRootComponent(ProjectileCollisionBox);
	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	ProjectileCollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnProjectileHit);


	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());
	//SyncNiagaraSpeed(UGA_OverClock::OverClockTimeScale);
	//UGA_OverClock::OnTimeScale.AddDynamic(this, &ABazierProjectile::SyncNiagaraSpeed);

	ExplosionRadius = 400.f;
	TimeToExplode = 3.f;
	
	bFire = false;
	FireSpeed = 1000.f;
	BazierWeight = 1.f;
	GroundOffset = 100.0f;
}

/*ABazierProjectile::~ABazierProjectile()
{
	UGA_OverClock::OnTimeScale.RemoveDynamic(this, &ABazierProjectile::SyncNiagaraSpeed);
}*/

void ABazierProjectile::FireInDirection(const FVector& ShootDirection)
{
	UE_LOG(LogTemp, Log, TEXT("FireInDirection"));
	FVector MyLocation = GetActorLocation();
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter)
	{
		OriginalTarget = PlayerCharacter;
		FVector TargetLocation = PlayerCharacter->GetActorLocation();
		TargetLocation.Z -= GroundOffset;
		SetBazierPoint(MyLocation, TargetLocation);
	}
}

void ABazierProjectile::Deflected(AActor* DeflectingActor)
{
	if (bIsDeflected || !GetInstigator())
		return;

	bIsDeflected = true;

	// 패링 당한 위치에서 투사체 발사한 Actor로 경로 설정
	FVector MyLocation = GetActorLocation();
	FVector ShooterLocation = GetInstigator()->GetActorLocation();
	ShooterLocation.Z -= GroundOffset;

	// 배지어 경로 재지정
	SetBazierPoint(MyLocation, ShooterLocation);

	// 타겟 변경
	OriginalTarget = GetInstigator();
}

void ABazierProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (UGA_OverClock::bIsOverClockActive)
	{
		CustomTimeDilation = UGA_OverClock::OverClockTimeScale;
	}
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

void ABazierProjectile::StickAndExplosion(const FHitResult& Hit)
{
	SetActorLocation(Hit.ImpactPoint);
	SetActorRotation(Hit.ImpactNormal.Rotation());

	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

	// 패링 당한 투사체라면 투사체를 발사한 Actor를 타겟으로 지정
	AActor* TargetActor = bIsDeflected ? GetInstigator() : UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (!TargetActor)
	{
		Destroy();
		return;
	}
	
	/*APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn == nullptr)
	{
		Destroy();
		return;
	}*/

	const float DistSq = FVector::DistSquared(TargetActor->GetActorLocation(), GetActorLocation());
	if (DistSq > FMath::Square(ExplosionRadius))
	{
		Destroy();
		return;
	}

	if (TargetActor->Implements<UAbilitySystemInterface>() && AttackDamageEffect)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

		// 패링된 투사체는 발사자를 플레이어로 지정함
		AActor* SourceActor = bIsDeflected ? UGameplayStatics::GetPlayerPawn(GetWorld(), 0) : GetInstigator();
		
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

/*
void ABazierProjectile::SyncNiagaraSpeed(float NewTimeScale)
{
	ProjectileNiagaraComponent->SetFloatParameter(FName("User.PlayRate"), UGA_OverClock::OverClockTimeScale);
}
*/
