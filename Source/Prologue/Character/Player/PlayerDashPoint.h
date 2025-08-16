// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerDashPoint.generated.h"

UCLASS()
class PROLOGUE_API APlayerDashPoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlayerDashPoint();

protected:
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*=======Shar Section=======*/
protected:
	// 수직 오프셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float VerticalOffset = 500.0f;

	// 안전 영역 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float SafeWeight = 100.0f;

private:
	UPROPERTY(VisibleAnywhere, Category="Dash")
	TObjectPtr<class AComma> Player;

	// 특정 각도만큼 회전시키는 함수
	FQuat RotateToWorld(const FQuat& From, const FQuat& To, float MaxRadian);


	/*=======Debug Section=======*/
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	bool bDrawDebug;

	/*=======Dash Section=======*/
public:
	// 검사 방향 변경 함수
	void SetDirection(FVector NewDirection, bool bConvertLocalToCameraDirection = true);

	// 대시 위치 리턴 함수 (보정과 함꼐 리턴)
	FVector GetPoint();

	// 회전 동기화 여부
	bool GetIsDirectionSync();

	// 주변 땅을 감지해 시야각이 가까운 땅 방향을 바라보게 설정
	void SetDirectionMinGround();

	// 대시 쿨타임 설정
	void SetDashCool();
	
	// 대시 쿨타임 리턴
	bool DashCoolDown();
	
protected:
	// 최대 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float MaxDistance = 1000.0f;

	// 대쉬 허용 시야각
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float FOVAngle = 60.0f;

	// 회전 검사 최대 각
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float RotationMaxDegreeAngle = 15.0f;

	// 검사 단위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	int32 PartialUnitCount = 64;

private:
	// 대쉬 방향
	UPROPERTY(EditAnywhere, Category = "Dash")
	FVector TargetDirection;
	UPROPERTY(EditAnywhere, Category = "Dash")
	FVector CurrentDirection;

	// 대시 쿨타임
	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashCool;
	
	// 현재 대시 쿨타임
	UPROPERTY(VisibleAnywhere, Category="Dash")
	float CurrentDashCool;

	// 대시 위치
	FVector DashPoint;

	// 이전 대쉬 대상 지면 액터
	AActor* GroundActor;

	// 대쉬 탐색 함수
	void CheckNewDirecionPoint();


	/*=======Parry Section=======*/
public:
	// 패리 위치 리턴 함수
	FVector GetParryPoint();

	// 패리 방향 동기화 여부 리턴
	bool GetIsParrySync();

	// 방향업데이트 중지 여부
	void SetCursorDirectionState(bool bState);

protected:
	// 최대 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parry")
	float ParryMaxDistance = 800.0f;

	// 회전 검사 최대 각
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parry")
	float ParryRotationMaxDelta = 15.0f;

	// 검사 단위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parry")
	int32 ParryPartialUnit = 32;

private:
	// 패리 방향 업데이트 불리언
	bool bParrySync;

	// 패리 방향
	UPROPERTY(VisibleAnywhere, Category = "Parry")
	FVector ParryCursorDirection;
	UPROPERTY(VisibleAnywhere, Category = "Parry")
	FVector ParryDirection;

	// 패리 돌진 위치
	FVector ParryPoint;

	// 이전 패리 대상 지면
	AActor* ParryGroundActor;

	void CheckParryDirectionPoint();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bShowDebug = true;
};
