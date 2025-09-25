// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_SpawnBazierProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Prologue/Character/Enemy/PrologueEnemyCharacter.h"
#include "Prologue/Weapon/Projectile/BazierProjectile.h"
#include "Prologue/Weapon/Projectile/EggBallProjectile.h"

UGA_SpawnBazierProjectile::UGA_SpawnBazierProjectile()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_SpawnBazierProjectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	USkeletalMeshComponent* MeshComp = nullptr;

	if (ACharacter* Character = CastChecked<ACharacter>(CurrentActorInfo->AvatarActor.Get()))
	{
		MeshComp = Character->GetMesh();
	}

	FVector SpawnLocation = MeshComp->GetSocketLocation(SocketName);

	FVector FireDirection;

	bool bTargetFoundAndUsed = false;

	// LaunchDirectionType에 따라서 투사체 발사 형태가 달라짐
	switch (LaunchDirectionType)
	{
		// Actor의 ForwardVector로 발사
		case ELaunchDirectionType::Forward:
			FireDirection = AvatarActor->GetActorForwardVector();
			break;

		// BP에서 지정해둔 Local Direction 방향으로 발사
		case ELaunchDirectionType::CustomLocalDirection:
			FireDirection = AvatarActor->GetActorTransform().TransformVectorNoScale(CustomLaunchDirection.GetSafeNormal());
			break;

		// 감지된 TargetActor가 있다면 BP에서 지정해둔 좌표의 방향으로 발사
		case ELaunchDirectionType::FindTargetProjectile:
			if (bFindTarget)
			{
				FVector TargetLocation;
				AActor* FoundTarget = FindTarget(AvatarActor, TargetLocation);

				if (FoundTarget)
				{
					FireDirection = (TargetLocation - SpawnLocation).GetSafeNormal();
					bTargetFoundAndUsed = true;
				}
				// TargetActor가 없을 때에는 ForwardVector로 발사
				else
				{
					FireDirection = AvatarActor->GetActorForwardVector();
				}
			}
			else
			{
				FireDirection = AvatarActor->GetActorForwardVector();
			}
			break;

		default:
			FireDirection = AvatarActor->GetActorForwardVector();
			break;
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = AvatarActor;
	SpawnParams.Instigator = Cast<APawn>(AvatarActor);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (NumProjectiles <= 0)
		NumProjectiles = 1;

	TSubclassOf<AActor> ChosenProjectileClass = ProjectileClass;
	bool bIsPerfectShot = false;

	// PerfectShotRequiredTag가 있고, PerfectProjectileClass가 할당 되어 있다면 PerfectShot 전용 투사체 발사
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AvatarActor))
	{
		if (PerfectShotRequiredTag.IsValid() && ASC->HasMatchingGameplayTag(PerfectShotRequiredTag))
		{
			if (PerfectProjectileClass)
			{
				ChosenProjectileClass = PerfectProjectileClass;
				bIsPerfectShot = true;
				LOG_SCREEN_R("Using PerfectShot Projectile");
			}
			else
			{
				LOG_SCREEN_R("PerfectProjectileClass is Not Set");
			}
		}
	}

	// 발사하는 투사체를 2개 이상으로 지정했다면 Cone 모양으로 흩뿌리게 함
	for (int32 i = 0; i < NumProjectiles; ++i)
	{
		FVector FinalShootDirection = FireDirection;

		if (NumProjectiles > 1 && SpreadHalfAngle > KINDA_SMALL_NUMBER)
		{
			FinalShootDirection = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(FireDirection, SpreadHalfAngle);
		}

		FRotator SpawnRotation = FireDirection.Rotation();
		AActor* SpawnActor = GetWorld()->SpawnActor<AActor>(ChosenProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

		if (SpawnActor)
		{
			if (AEggBallProjectile* Projectile = Cast<AEggBallProjectile>(SpawnActor))
			{
				Projectile->FireInDirection(FinalShootDirection);
			}
			else if (ABazierProjectile* Bazier = Cast<ABazierProjectile>(SpawnActor))
			{
				Bazier->FireInDirection(FinalShootDirection);
			}
		}
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

// LaunchDirectionType의 타입이 FindTargetProjectile일 때, 감지된 Actor들 중 가장 가까운 Actor를 찾아주는 함수
AActor* UGA_SpawnBazierProjectile::FindTarget(AActor* AvatarActor, FVector& OutTargetLocation) const
{
	AActor* BestTarget = nullptr;

	float MinDistSq = FMath::Square(TargetSearchRadius);

	TArray<AActor*> FoundTarget;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APrologueEnemyCharacter::StaticClass(), FoundTarget);

	FVector AvatarLocation = AvatarActor->GetActorLocation();
	FVector AvatarForward = AvatarActor->GetActorForwardVector();

	for (AActor* TargetActor : FoundTarget)
	{
		if (!TargetActor || TargetActor == AvatarActor || !TargetActor->Implements<UAbilitySystemInterface>())
		{
			continue;
		}

		FVector DirectionToTarget = (TargetActor->GetActorLocation() - AvatarLocation);
		float DistSq = DirectionToTarget.SizeSquared();

		if (DistSq < MinDistSq)
		{
			if (TargetSearchAngle < 360.f)
			{
				DirectionToTarget.Normalize();
				float AngleRad = FMath::Acos(FVector::DotProduct(AvatarForward, DirectionToTarget));
				float AngleDeg = FMath::RadiansToDegrees(AngleRad);

				if (AngleDeg > TargetSearchAngle / 2.f)
				{
					continue;
				}
			}

			MinDistSq = DistSq;
			BestTarget = TargetActor;
		}
	}

	if (BestTarget)
	{
		OutTargetLocation = BestTarget->GetActorLocation();
	}

	return BestTarget;
}
