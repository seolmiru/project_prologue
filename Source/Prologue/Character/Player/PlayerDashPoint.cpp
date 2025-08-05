// Fill out your copyright notice in the Description page of Project Settings.


#include "Prologue/Character/Player/PlayerDashPoint.h"

#include "Comma.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/Controller/CommaController.h"

// Sets default values
APlayerDashPoint::APlayerDashPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bParrySync = true;

	TargetDirection = FVector::ForwardVector;
	CurrentDirection = TargetDirection;
	DashPoint = FVector::ForwardVector;

	ParryCursorDirection = FVector::ForwardVector;
	ParryDirection = ParryCursorDirection;
	ParryPoint = FVector::ForwardVector;
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

	// Dash Section
	CheckNewDirecionPoint();

	// Parry Section
	if (bParrySync && Player)
	{
		ParryCursorDirection = Player->GetMouseDirection();
	}
	CheckParryDirectionPoint();

	/*// 디버깅
	FColor DrawColor = FColor::Green;
	DrawDebugSphere(
		GetWorld(),
		ParryPoint,
		50.f,
		12,
		DrawColor,
		false,
		-1.f,
		0,
		2.f
	);*/
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
	FVector ResultPoint = DashPoint;

	FVector Direction = DashPoint - Player->GetActorLocation();
	Direction.Z = 0.0f;
	Direction.Normalize();

	FHitResult ForwardResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Player);
	FVector ForwardStartPoint = DashPoint + (Direction * SafeWeight);
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
	FVector RearStartPoint = DashPoint - (Direction * SafeWeight);
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

void APlayerDashPoint::SetDirectionMinGround()
{
	FVector PlayerLocation = Player->GetActorLocation();

	// 충돌 대상 타입 정의
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel7));

	// 결과 배열
	TArray<AActor*> OverlapActors;

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

	bool bPlayerHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		PlayerLocation,
		PlayerFloorEnd,
		FQuat::Identity,
		ECC_GameTraceChannel8,
		FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight),
		Params
	);

	// 무시할 대상
	TArray<AActor*> OverlapIgnore;
	OverlapIgnore.Add(Player); // 플레이어
	OverlapIgnore.Add(HitResult.GetActor()); // 플레이어가 서있는 지면

	bool bGroundHit = UKismetSystemLibrary::CapsuleOverlapActors(
		GetWorld(),
		PlayerLocation,
		MaxDistance,
		VerticalOffset,
		ObjectTypes,
		nullptr,
		OverlapIgnore,
		OverlapActors
	);

	// 주변 지면 존재시 실행
	if (bGroundHit)
	{
		// 플레이어 전방
		FVector PlayerForward = Player->GetActorForwardVector();
		PlayerForward.Z = 0.0f;
		PlayerForward.Normalize();

		float LowAngle = 360.0f;
		FVector LowDirection = CurrentDirection;

		for (int i = 0; i < OverlapActors.Num(); i++)
		{
			FVector GroundLocation = OverlapActors[i]->GetActorLocation();
			FVector GroundDirection = GroundLocation - PlayerLocation;
			GroundDirection.Z = 0.0f;
			GroundDirection.Normalize();

			float Radian = FMath::Atan2(
				FVector::CrossProduct(PlayerForward, GroundDirection).Z,
				FVector::DotProduct(PlayerForward, GroundDirection)
			);
			float Degree = FMath::RadiansToDegrees(Radian);
			Degree = FMath::Abs(Degree);

			if (Degree <= FOVAngle && Degree < LowAngle)
			{
				LowDirection = GroundDirection;
				LowAngle = Degree;
			}
		}

		CurrentDirection = LowDirection;
	}
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
	/*FColor DrawColor = bPlayerHit ? FColor::Green : FColor::Red;

	DrawDebugCapsule(
		GetWorld(),
		HitResult.ImpactPoint,
		CapsuleHalfHeight,
		CapsuleRadius,
		FQuat::Identity,
		DrawColor,
		false,
		-1.0f,
		0,
		2.0f
	);*/

	// 새로운 이동 위치 탐색
	if (bPlayerHit)
	{
		AActor* PlayerGround = HitResult.GetActor();

		// 현재 설정 위치의 시야각 계산
		FVector OldDirection = DashPoint - PlayerLocation;
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

		// 네비 확인 변수
		UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

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

			/*DrawColor = bHit ? FColor::Green : FColor::Red;
			DrawDebugSphere(
				GetWorld(),
				DashPoint,
				50.f,
				12,
				DrawColor,
				false,
				-1.f,
				0,
				2.f
			);*/

			// 충돌시 실행
			if (bHit)
			{
				FNavLocation NavLocation;
				bool bOnNav = NavSystem->ProjectPointToNavigation(HitResult.ImpactPoint, NavLocation,
				                                                  FVector(50, 50, 100));
				if (bOnNav)
				{
					// =======================================
					// 새 충돌 지점 지면
					// =======================================
					AActor* HitGround = HitResult.GetActor();

					// =======================================
					// 현재 이동 위치와 플레이어의 거리
					// =======================================
					float CurrentDistance = FVector2D::Distance((FVector2d)PlayerLocation,
					                                            (FVector2d)DashPoint);

					// =======================================
					// 새 위치 시야각
					// =======================================

					// 방향벡터
					FVector NewDirection = NavLocation.Location - PlayerLocation;
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
					if (HitGround != PlayerGround)
					{
						// 현재 이동 대상 지면과 같은 지면일 경우 
						if (GroundActor != nullptr || HitGround == GroundActor)
						{
							float NewDistance = FVector2D::Distance((FVector2d)PlayerLocation,
							                                        (FVector2d)HitResult.ImpactPoint);

							if ((NewDistance > CurrentDistance && NewDistance <= MaxDistance) //조건 1: 더 먼 거리로 이동
								|| NewDegreeAngle < OldDegreeAngle) // 조건 2: 정면이랑 더 가까운 방향으로 이동
							{
								//위치 재설정
								GroundActor = HitGround;
								DashPoint = NavLocation.Location;
								break;
							}
						}
						// 새로운 지면일 경우 해당 위치로 설정
						else
						{
							GroundActor = HitGround;
							DashPoint = NavLocation.Location;
							break;
						}
					}
					// =======================================
					// 플레이어가 서있는 지면과 충돌
					// =======================================
					else
					{
						float NewDistance = FVector2D::Distance((FVector2d)PlayerLocation,
						                                        (FVector2d)NavLocation.Location);
						// 조건 충족시 현재 충돌된 지점으로 대시 위치 변경
						if (OldDegreeAngle > FOVAngle // 조건 1: 현재 위치가 시야각을 벗어날 경우
							|| CurrentDistance > MaxDistance // 조건 2: 현재 위치가 최대 거리보다 멀 경우
							|| NewDistance > CurrentDistance // 조건 3: 새 위치가 더 멀리 이동 가능할경우
							|| (OldDegreeAngle > NewDegreeAngle && GroundActor == PlayerGround))
						// 조건 4: 새로운 위치가 정면 방향에 더 가까울 경우 (플레이어와 같은 지면에 한하여)
						{
							GroundActor = HitGround;
							DashPoint = NavLocation.Location;
							break;
						}
					}
				}
			} // End bHit

			// 검사 위치 땡기기
			CurrentCheckLocation -= TargetDirection * UnitDistance;
		}
	}
}

FVector APlayerDashPoint::GetParryPoint()
{
	FVector ResultPoint = ParryPoint;

	FVector Direction = ParryPoint - Player->GetActorLocation();
	Direction.Z = 0.0f;
	Direction.Normalize();

	FHitResult ForwardResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Player);
	FVector ForwardStartPoint = ParryPoint + (Direction * SafeWeight);
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
	FVector RearStartPoint = ParryPoint - (Direction * SafeWeight);
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

bool APlayerDashPoint::GetIsParrySync()
{
	float RadianAngle = FMath::Atan2(
		FVector::CrossProduct(ParryDirection, ParryCursorDirection).Z,
		FVector::DotProduct(ParryDirection, ParryCursorDirection)
	);
	float DegreeAngle = FMath::RadiansToDegrees(RadianAngle);
	DegreeAngle = FMath::Abs(DegreeAngle);
	return DegreeAngle < 5;
}

void APlayerDashPoint::SetCursorDirectionState(bool bState)
{
	bParrySync = bState;
}

void APlayerDashPoint::CheckParryDirectionPoint()
{
	// =======================================
	// 목표 방향을 향해 회전
	// =======================================
	if (!GetIsParrySync())
	{
		// Quaternion 변환
		FQuat MyQuat = FRotationMatrix::MakeFromX(ParryDirection).ToQuat();
		FQuat TargetQuat = FRotationMatrix::MakeFromX(ParryCursorDirection).ToQuat();

		// 최대 회전 각도 (라디안)
		float MaxRadian = FMath::DegreesToRadians(ParryRotationMaxDelta);

		// 각도 계산
		ParryDirection = RotateToWorld(MyQuat, TargetQuat, MaxRadian).GetForwardVector();
	}

	FVector PlayerLocation = Player->GetActorLocation(); // 플레이어 위치
	FVector CurrentCheckLocation = PlayerLocation + ParryDirection * ParryMaxDistance; // 라인 트레이스 검사 시작 위치
	float UnitDistance = ParryMaxDistance / ParryPartialUnit; // 유닛 거리 단위

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

	bool bPlayerHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		PlayerLocation,
		PlayerFloorEnd,
		FQuat::Identity,
		ECC_GameTraceChannel8,
		FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight),
		Params
	);

	// 새로운 이동 위치 탐색
	if (bPlayerHit)
	{
		AActor* PlayerGround = HitResult.GetActor();

		// 현재 설정 위치의 시야각 계산
		FVector OldDirection = ParryPoint - PlayerLocation;
		OldDirection.Z = 0.0f;
		OldDirection.Normalize();

		float OldRadianAngle = FMath::Atan2(
			FVector::CrossProduct(ParryDirection, OldDirection).Z,
			FVector::DotProduct(ParryDirection, OldDirection)
		);
		float OldDegreeAngle = FMath::RadiansToDegrees(OldRadianAngle);
		OldDegreeAngle = FMath::Abs(OldDegreeAngle);

		// 네비 관련 설정
		UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

		for (int i = 0; i < ParryPartialUnit; i++)
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

			// 충돌시 실행
			if (bHit)
			{
				FNavLocation NavLocation;
				bool bOnNav = NavSystem->ProjectPointToNavigation(HitResult.ImpactPoint, NavLocation,
				                                                  FVector(50, 50, 100));

				if (bOnNav)
				{
					// =======================================
					// 새 충돌 지점 지면
					// =======================================
					AActor* HitGround = HitResult.GetActor();

					// =======================================
					// 현재 이동 위치와 플레이어의 거리
					// =======================================
					float CurrentDistance = FVector2D::Distance((FVector2d)PlayerLocation,
					                                            (FVector2d)ParryPoint);

					// =======================================
					// 새 위치 시야각
					// =======================================

					// 방향벡터
					FVector NewDirection = HitResult.ImpactPoint - PlayerLocation;
					NewDirection.Z = 0.0f;
					NewDirection.Normalize();

					float NewRadianAngle = FMath::Atan2(
						FVector::CrossProduct(ParryDirection, NewDirection).Z,
						FVector::DotProduct(ParryDirection, NewDirection)
					);
					float NewDegreeAngle = FMath::RadiansToDegrees(NewRadianAngle);
					NewDegreeAngle = FMath::Abs(NewDegreeAngle);

					// =======================================
					// 플레이어가 서있지 않은 지면과 충돌
					// =======================================
					if (HitGround != PlayerGround)
					{
						// 현재 이동 대상 지면과 같은 지면일 경우 
						if (ParryGroundActor != nullptr || HitGround == ParryGroundActor)
						{
							float NewDistance = FVector2D::Distance((FVector2d)PlayerLocation,
							                                        (FVector2d)HitResult.ImpactPoint);

							if ((NewDistance > CurrentDistance && NewDistance <= ParryMaxDistance) //조건 1: 더 먼 거리로 이동
								|| NewDegreeAngle < OldDegreeAngle) // 조건 2: 정면이랑 더 가까운 방향으로 이동
							{
								//위치 재설정
								ParryGroundActor = HitGround;
								ParryPoint = HitResult.ImpactPoint;
								break;
							}
						}
						// 새로운 지면일 경우 해당 위치로 설정
						else
						{
							ParryGroundActor = HitGround;
							ParryPoint = HitResult.ImpactPoint;
							break;
						}
					}
					// =======================================
					// 플레이어가 서있는 지면과 충돌
					// =======================================
					else
					{
						// 조건 충족시 현재 충돌된 지점으로 대시 위치 변경
						// if (OldDegreeAngle > FOVAngle // 조건 1: 현재 위치가 시야각을 벗어날 경우
						if (CurrentDistance > MaxDistance // 조건 2: 현재 위치가 최대 거리보다 멀 경우
							|| (OldDegreeAngle > NewDegreeAngle && GroundActor == PlayerGround))
						// 조건 3: 새로운 위치가 정면 방향에 더 가까울 경우 (플레이어와 같은 지면에 한하여)
						{
							ParryGroundActor = HitGround;
							ParryPoint = HitResult.ImpactPoint;
							break;
						}
					}
				}
			}

			// 검사 위치 땡기기
			CurrentCheckLocation -= TargetDirection * UnitDistance;
		}
	}
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
