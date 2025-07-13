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

void APlayerDashPoint::SetDirection(FVector NewDirection, bool bConvertLocalToCameraDirection)
{
	// 카메라 회전을 기준으로 월드 좌표계 방향 계산
	FVector WorldDirection = NewDirection;

	if (bConvertLocalToCameraDirection)
	{
		FRotator ControlRot = Player->GetController()->GetControlRotation();
		FRotator YawRotation(0, ControlRot.Yaw, 0);
		FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		WorldDirection = ForwardVector * NewDirection.Y + RightVector * NewDirection.X;
	}

	WorldDirection.Normalize();

	if (WorldDirection != TargetDirection)
	{
		TargetDirection = WorldDirection;
	}
}

FVector APlayerDashPoint::GetPoint()
{
	FVector ResultPoint = Point;
	
	FVector Direction = Point - Player->GetActorLocation();
	Direction.Z = 0.0f;
	Direction.Normalize();

	FHitResult ForwardResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Player);
	FVector ForwardStartPoint = Point + (Direction * SafeWeight);
	FVector ForwardEndPoint = ForwardStartPoint;
	ForwardStartPoint.Z += VerticalOffset;
	ForwardEndPoint.Z -= VerticalOffset;

	bool bForward = GetWorld()->LineTraceSingleByChannel(
		ForwardResult,
		ForwardStartPoint,
		ForwardEndPoint,
		ECC_GameTraceChannel8,
		Params
		);

	FHitResult RearResult;
	FVector RearStartPoint = Point - (Direction * SafeWeight);
	FVector RearEndPoint = RearStartPoint;
	RearStartPoint.Z += VerticalOffset;
	RearEndPoint.Z -= VerticalOffset;
	
	bool bRear = GetWorld()->LineTraceSingleByChannel(
		RearResult,
		RearStartPoint,
		RearEndPoint,
		ECC_GameTraceChannel8,
		Params
	);

	// 지면 끝에 걸쳐있다면 지면 안쪽으로 보정
	if (bForward && !bRear) // 앞쪽으로 보정
	{
		ResultPoint = ForwardResult.ImpactPoint;
	}
	else if (!bForward && bRear) // 뒤쪽으로 보정
	{
		ResultPoint = RearResult.ImpactPoint;
	}
	
	return ResultPoint;
}

bool APlayerDashPoint::GetIsDirectionSync()
{
	float RadianAngle = FMath::Atan2(
		FVector::CrossProduct(CurrentDirection, TargetDirection).Z,
		FVector::DotProduct(CurrentDirection, TargetDirection)
	);
	float DegreeAngle = FMath::RadiansToDegrees(RadianAngle);
	DegreeAngle = FMath::Abs(DegreeAngle);
	return DegreeAngle < 5;
}

void APlayerDashPoint::CheckNewDirecionPoint()
{
	// =======================================
	// 목표 방향을 향해 회전
	// =======================================
	if (!GetIsDirectionSync())
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

	// =======================================
	// 플레이어 지면 검사
	// =======================================
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Player);

	FVector PlayerFloorStart = PlayerLocation;
	FVector PlayerFloorEnd = PlayerLocation;
	PlayerFloorEnd.Z -= Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 50.0f;
	const float CapsuleRadius = Player->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const float CapsuleHalfHeight = Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	// bool bPlayerHit = GetWorld()->LineTraceSingleByChannel(
	// 	HitResult,
	// 	PlayerFloorStart,
	// 	PlayerFloorEnd,
	// 	ECC_GameTraceChannel8,
	// 	Params
	// );

	bool bPlayerHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		PlayerLocation,
		PlayerFloorEnd,
		FQuat::Identity,
		ECC_GameTraceChannel8,
		FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight),
		Params
	);

	// 디버깅
#pragma region Debug
	FColor DrawColor = bPlayerHit ? FColor::Green : FColor::Red;

	DrawDebugCapsule(
		GetWorld(),
		PlayerLocation,
		CapsuleHalfHeight,
		CapsuleRadius,
		FQuat::Identity,
		DrawColor,
		false,
		 -1.0f,
		 0,
		 2.0f
	);
#pragma endregion

	// 새로운 이동 위치 탐색
	if (bPlayerHit)
	{
		AActor* PlayerGround = HitResult.GetActor();

		// 현재 설정 위치의 시야각 계산
#pragma region Current FOV Angle
		FVector OldDirection = Point - PlayerLocation;
		OldDirection.Z = 0.0f;
		OldDirection.Normalize();

		FVector PlayerForward = Player->GetActorForwardVector();
		PlayerForward.Z = 0.0f;
		PlayerForward.Normalize();

		float OldRadianAngle = FMath::Atan2(
			FVector::CrossProduct(PlayerForward, OldDirection).Z,
			FVector::DotProduct(PlayerForward, OldDirection)
		);
		float OldDegreeAngle = FMath::RadiansToDegrees(OldRadianAngle);
		OldDegreeAngle = FMath::Abs(OldDegreeAngle);
#pragma endregion

		for (int i = 0; i < PartialUnitCount; i++)
		{
			FVector Start = CurrentCheckLocation;
			Start.Z += VerticalOffset;

			FVector End = CurrentCheckLocation;
			End.Z -= VerticalOffset;

			bool bHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				Start,
				End,
				ECC_GameTraceChannel8,
				Params
			);

#pragma region Debug
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
				2.f
			);
#pragma endregion

			// 충돌시 실행
			if (bHit)
			{
				// =======================================
				// 새 충돌 지점 지면
				// =======================================
				AActor* HitGround = HitResult.GetActor();

				// =======================================
				// 현재 이동 위치와 플레이어의 거리
				// =======================================
				float CurrentDistance = FVector2D::Distance((FVector2d)PlayerLocation,
				                                            (FVector2d)Point);

				// =======================================
				// 새 위치 시야각
				// =======================================

				// 방향벡터
				FVector NewDirection = HitResult.ImpactPoint - PlayerLocation;
				NewDirection.Z = 0.0f;
				NewDirection.Normalize();

				float NewRadianAngle = FMath::Atan2(
					FVector::CrossProduct(PlayerForward, NewDirection).Z,
					FVector::DotProduct(PlayerForward, NewDirection)
				);
				float NewDegreeAngle = FMath::RadiansToDegrees(NewRadianAngle);
				NewDegreeAngle = FMath::Abs(NewDegreeAngle);

				// =======================================
				// 플레이어가 서있지 않은 지면과 충돌
				// =======================================
				if (GroundActor != nullptr && HitGround != PlayerGround)
				{
					UE_LOG(LogTemp, Log, TEXT("Another Player Ground Hit: %s / %s"), *HitGround->GetName(),
					       *PlayerGround->GetName());

					// 현재 이동 대상 지면과 같은 지면일 경우 
					if (HitGround == GroundActor)
					{
						float NewDistance = FVector2D::Distance((FVector2d)PlayerLocation,
						                                        (FVector2d)HitResult.ImpactPoint);

						if ((NewDistance > CurrentDistance && NewDistance <= MaxDistance) //조건 1: 더 먼 거리로 이동
							|| NewDegreeAngle < OldDegreeAngle) // 조건 2: 정면이랑 더 가까운 방향으로 이동
						{
							//위치 재설정
							GroundActor = HitGround;
							Point = HitResult.ImpactPoint;
							break;
						}
					}
					// 새로운 지면일 경우 해당 위치로 설정
					else
					{
						GroundActor = HitGround;
						Point = HitResult.ImpactPoint;
						break;
					}
				}
				// =======================================
				// 플레이어가 서있는 지면과 충돌
				// =======================================
				else
				{
					// 조건 충족시 현재 충돌된 지점으로 대시 위치 변경
					if (OldDegreeAngle > FOVAngle // 조건 1: 현재 위치가 시야각을 벗어날 경우
						|| CurrentDistance > MaxDistance // 조건 2: 현재 위치가 최대 거리보다 멀 경우
						|| OldDegreeAngle > NewDegreeAngle) // 조건 3: 새로운 위치가 정면 방향에 더 가까울 경우
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

	// =======================================
	// 위치 업데이트 재개
	// =======================================
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
