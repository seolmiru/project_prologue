// Fill out your copyright notice in the Description page of Project Settings.


#include "Prologue/Character/Player/PlayerDashPoint.h"

#include "Comma.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/Controller/CommaController.h"

// Sets default values
APlayerDashPoint::APlayerDashPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bTickFlag = true;
	TargetDirection = FVector::ForwardVector;
	CurrentDirection = TargetDirection;
}

void APlayerDashPoint::BeginPlay()
{
	Super::BeginPlay();

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn != nullptr)
	{
		AComma* PlayerComma = Cast<AComma>(PlayerPawn);

		if (PlayerComma != nullptr)
		{
			Player = TObjectPtr<AComma>(PlayerComma);
			TargetDirection = Player->GetActorForwardVector();
		}
		else
		{
			LOG_SCREEN("Player Comma is Null");
		}
	}
	else
	{
		LOG_SCREEN("Player Pawn is Null");
	}
}

// Called every frame
void APlayerDashPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 대시 위치 갱신
	if (bTickFlag)
	{
		CheckNewDirecionPoint();
	}
}

void APlayerDashPoint::SetDirection(FVector NewDirection)
{
	// 카메라 회전을 기준으로 월드 좌표계 방향 계산
	FRotator ControlRot = Player->GetController()->GetControlRotation();
	FRotator YawRotation(0, ControlRot.Yaw, 0);
	FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	FVector WorldDirection = ForwardVector * NewDirection.Y + RightVector * NewDirection.X;
	WorldDirection.Normalize();

	if (WorldDirection != TargetDirection)
	{
		TargetDirection = WorldDirection;
	}
}

FVector APlayerDashPoint::GetPoint()
{
	return Point;
}

void APlayerDashPoint::CheckNewDirecionPoint()
{
	if (CurrentDirection != TargetDirection)
	{
		// Quaternion 변환
		FQuat MyQuat = FRotationMatrix::MakeFromX(CurrentDirection).ToQuat();
		FQuat TargetQuat = FRotationMatrix::MakeFromX(TargetDirection).ToQuat();

		// 최대 회전 각도 (라디안)
		float MaxRadian = FMath::DegreesToRadians(RotationMaxDegreeAngle);

		// 각도 계산
		CurrentDirection = RotateToWorld(MyQuat, TargetQuat, MaxRadian).GetForwardVector();
	}

	FVector PlayerLocation = Player->GetActorLocation(); // 플레이어 위치
	FVector CurrentCheckLocation = PlayerLocation + CurrentDirection * MaxDistance; // 라인 트레이스 검사 시작 위치
	float UnitDistance = MaxDistance / PartialUnitCount; // 유닛 거리 단위

	// 위치 업데이트 중지
	bTickFlag = false;

	// 플레이어 지면 검사
	FHitResult HitResult;
	FCollisionQueryParams Params;

	FVector PlayerFloorStart = PlayerLocation;
	FVector PlayerFloorEnd = PlayerLocation;
	PlayerFloorEnd.Z -= Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 50.0f;

	bool bPlayerHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		PlayerFloorStart,
		PlayerFloorEnd,
		ECC_Visibility,
		Params
	);
	AActor* PlayerGround = HitResult.GetActor();

	// 디버깅
	FColor DrawColor = bPlayerHit ? FColor::Green : FColor::Red;

	DrawDebugLine(
		GetWorld(),
		PlayerFloorStart,
		PlayerFloorEnd,
		DrawColor,
		false,
		-1.0f,
		0,
		2.0f
	);

	// 새로운 이동 위치 탐색
	if (bPlayerHit)
	{
		for (int i = 0; i < PartialUnitCount; i++)
		{
			FVector Start = CurrentCheckLocation;
			Start.Z += VerticalOffset;

			FVector End = CurrentCheckLocation;
			End.Z -= VerticalOffset;

			Params.AddIgnoredActor(this);

			bool bHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				Start,
				End,
				ECC_Visibility,
				Params
			);

			DrawColor = bHit ? FColor::Green : FColor::Red;
			DrawDebugSphere(
				GetWorld(),
				Point,
				50.f,
				12,
				DrawColor,
				false,
				-1.f,
				0,
				2.f);

			// 충돌시 실행
			if (bHit)
			{
				AActor* HitGround = HitResult.GetActor();
				float CurrentDistance = FVector2D::Distance((FVector2d)PlayerLocation,
				                                            (FVector2d)Point);

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
							UE_LOG(LogTemp, Log, TEXT("Same Ground Hit: Far Distance"))
							GroundActor = HitGround;
							Point = HitResult.ImpactPoint;
						}
					}
					// 새로운 지면일 경우 해당 위치로 설정
					else
					{
						UE_LOG(LogTemp, Log, TEXT("Another Ground Hit: Far Distance"))
						GroundActor = HitGround;
						Point = HitResult.ImpactPoint;
					}

					break;
				}
				// 플레이어가 서있는 지면과 충돌
				else
				{
					FVector MyDirection = PlayerLocation - Point;
					MyDirection.Z = 0.0f;

					float RadianAngle = FMath::Acos(
						FVector::DotProduct(Player->GetActorForwardVector(), MyDirection.GetSafeNormal()));
					float DegreeAngle = FMath::RadiansToDegrees(RadianAngle);

					//현재 대쉬 위치가 시야각을 벗어나거나 최대 거리 이상일 경우 위치 재설정
					if (DegreeAngle * 2.0f > FOVAngle || CurrentDistance > MaxDistance)
					{
						GroundActor = HitGround;
						Point = HitResult.ImpactPoint;
						break;
					}
				}
			}

			// 검사 위치 땡기기
			CurrentCheckLocation -= TargetDirection * UnitDistance;
		}
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
