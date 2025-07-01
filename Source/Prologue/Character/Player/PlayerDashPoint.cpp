// Fill out your copyright notice in the Description page of Project Settings.


#include "Prologue/Character/Player/PlayerDashPoint.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerDashPoint::APlayerDashPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bTickFlag = true;
	Direction = FVector::ForwardVector;
	Point = GetActorLocation();

	Player = TObjectPtr<AActor>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

// Called every frame
void APlayerDashPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 대시 위치 갱신
	CheckNewDirecionPoint(Direction);
}

void APlayerDashPoint::SetDirection(FVector NewDirection)
{
	if (NewDirection != Direction)
	{
		// Quaternion 변환
		FQuat MyQuat = FRotationMatrix::MakeFromX(Direction).ToQuat();
		FQuat TargetQuat = FRotationMatrix::MakeFromX(NewDirection).ToQuat();

		// 최대 회전 각도 (라디안)
		float MaxRadian = FMath::DegreesToRadians(RotationMaxDegreeAngle);
		// 최대 회전 회수
		int MaxTryCount = 360.0f / RotationMaxDegreeAngle;

		for (int i = 0; i < MaxTryCount; i++)
		{
			FQuat ResultQuat = RotateToWorld(MyQuat, TargetQuat, MaxRadian);
			CheckNewDirecionPoint(ResultQuat.GetForwardVector()); // 새 방향 이동지점 검사

			if (Direction == NewDirection)
				break;

			MyQuat = FRotationMatrix::MakeFromX(Direction).ToQuat(); // 변수 업데이트
		}
	}
}

void APlayerDashPoint::CheckNewDirecionPoint(FVector NewDirection)
{
	Direction = NewDirection;

	FVector PlayerLocation = Player->GetActorLocation(); // 플레이어 위치
	FVector CurrentCheckLocation = Direction * MaxDistance; // 라인 트레이스 검사 시작 위치
	float UnitDistance = MaxDistance / PartialUnitCount; // 유닛 거리 단위

	// 위치 업데이트 중지
	bTickFlag = false;

	// 새로운 이동 위치 탐색
	for (int i = 0; i < PartialUnitCount; i++)
	{
		FHitResult HitResult;
		FVector Start = CurrentCheckLocation;
		Start.Z += VerticalOffset;

		FVector End = CurrentCheckLocation;
		End.Z -= VerticalOffset;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		FVector PlayerFloorStart = PlayerLocation;
		PlayerFloorStart.Z += 100.0f;
		FVector PlayerFloorEnd = PlayerLocation;
		PlayerFloorEnd.Z -= 100.0f;

		bool bPlayerHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			PlayerFloorStart,
			PlayerFloorEnd,
			ECC_Visibility,
			Params
		);

		// 플레이어 지면 검사
		if (bPlayerHit)
		{
			AActor* PlayerGround = HitResult.GetActor();

			bool bHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				Start,
				End,
				ECC_Visibility,
				Params
			);

			// 충돌시 실행
			if (bHit)
			{
				AActor* HitGround = HitResult.GetActor();
				float CurrentDistance = FVector2D::Distance((FVector2d)PlayerLocation,
															(FVector2d)GetActorLocation());

				// 플레이어가 서있지 않은 지면과 충돌
				if (GroundActor != nullptr && HitGround != PlayerGround)
				{
					// 현재 이동 대상 지면과 같은 위치일경우 더 먼 거리의 위치로 설정 
					if (HitGround == GroundActor)
					{
						float NewDistance = FVector2D::Distance((FVector2d)PlayerLocation,
						                                        (FVector2d)HitResult.ImpactPoint);

						// 새로운 위치가 현재 위치보다 더 멀리 이동 가능할 때
						if (NewDistance > CurrentDistance && NewDistance <= MaxDistance)
						{
							//위치 재설정
							SetActorLocation(HitResult.ImpactPoint);
						}
					}
					// 새로운 지면일 경우 해당 위치로 설정
					else
					{
						SetActorLocation(HitResult.ImpactPoint);
					}
					
					break;
				}
				// 플레이어가 서있는 지면과 충돌
				else
				{
					FVector MyDirection = PlayerLocation - GetActorLocation();
					MyDirection.Z = 0.0f;

					float RadianAngle = FMath::Acos(FVector::DotProduct(Player->GetActorForwardVector(), MyDirection.GetSafeNormal()));
					float DegreeAngle = FMath::RadiansToDegrees(RadianAngle);

					//현재 대쉬 위치가 시야각을 벗어나거나 최대 거리 이상일 경우 위치 재설정
					if (DegreeAngle * 2.0f > FOVAngle || CurrentDistance > MaxDistance)
					{
						SetActorLocation(HitResult.ImpactPoint);
						break;
					}
				}
			}
		}

		// 검사 위치 땡기기
		CurrentCheckLocation -= Direction * UnitDistance;
	}

	
	// 위치 업데이트 재개
	bTickFlag = true;
}

FQuat APlayerDashPoint::RotateToWorld(const FQuat& From, const FQuat& To, float MaxRadian)
{
	float Angle = From.AngularDistance(To); // 현재와 목표 회전 사이의 각도

	if (Angle < KINDA_SMALL_NUMBER)
	{
		return To; // 이미 거의 같은 방향
	}

	float T = FMath::Min(1.f, MaxRadian / Angle); // 전체 회전 중 몇 %만 할지
	return FQuat::Slerp(From, To, T);
}
