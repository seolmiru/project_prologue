// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_SpawnProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Prologue/Character/Enemy/PrologueEnemyCharacter.h"
#include "Prologue/Weapon/Projectile/PrologueProjectileBase.h"

UGA_SpawnProjectile::UGA_SpawnProjectile()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_SpawnProjectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
	
	switch (LaunchDirectionType)
	{
		case ELaunchDirectionType::Forward:
			FireDirection = AvatarActor->GetActorForwardVector();
			break;

		case ELaunchDirectionType::CustomLocalDirection:
			FireDirection = AvatarActor->GetActorTransform().TransformVectorNoScale(CustomLaunchDirection.GetSafeNormal());
			break;

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

	
	for (int32 i = 0; i < NumProjectiles; ++i)
	{
		FVector FinalShootDirection = FireDirection;

		if (NumProjectiles > 1 && SpreadHalfAngle > KINDA_SMALL_NUMBER)
		{
			FinalShootDirection = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(FireDirection, SpreadHalfAngle);
		}

		FRotator SpawnRotation = FireDirection.Rotation();
		AActor* SpawnActor = GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

		if (SpawnActor)
		{
			if (APrologueProjectileBase* Projectile = Cast<APrologueProjectileBase>(SpawnActor))
			{
				Projectile->FireInDirection(FinalShootDirection);
			}
		}
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

AActor* UGA_SpawnProjectile::FindTarget(AActor* AvatarActor, FVector& OutTargetLocation) const
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
