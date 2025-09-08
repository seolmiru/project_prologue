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
	virtual void Tick(float DeltaTime) override;

protected:
	// 보정 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Total")
	float SearchRadius = 200.0f;

	FVector GetNoMonsterGround(FVector OriginPoint);
	
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
	bool GetDashCoolState();

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

	// 문 오브젝트 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	TArray<TEnumAsByte<EObjectTypeQuery>> GateObjectType;
	
	// 문에 막혔을 때의 안전거리 오프셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float GateSafetyOffset = 50.f;

private:
	// 대쉬 방향
	UPROPERTY(EditAnywhere, Category = "Dash")
	FVector TargetDirection;
	UPROPERTY(EditAnywhere, Category = "Dash")
	FVector CurrentDirection;

	// 대시 위치
	FVector DashPoint;

	// 이전 대쉬 대상 지면 액터
	AActor* GroundActor;

	// 대쉬 탐색 함수
	void CheckNewDirecionPoint();


	/*=======Parry Section=======*/
public:
	// 스킬 위치 리턴 함수
	FVector GetSkillPoint();

	// 스킬 방향 동기화 여부 리턴
	bool GetIsSkillSync();

	// 방향업데이트 중지 여부
	void SetCursorDirectionState(bool bState);

protected:
	// 최대 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parry")
	float SkillMaxDistance = 800.0f;

	// 회전 검사 최대 각
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parry")
	float SkillRotationMaxDelta = 15.0f;

	// 검사 단위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parry")
	int32 SkillPartialUnit = 32;

private:
	// 스킬 방향 업데이트 불리언
	bool bSkillSync;

	// 스킬 방향
	UPROPERTY(VisibleAnywhere, Category = "Parry")
	FVector SkillCursorDirection;
	UPROPERTY(VisibleAnywhere, Category = "Parry")
	FVector SkillDirection;

	// 스킬 돌진 위치
	FVector SkillPoint;

	// 이전 스킬 대상 지면
	AActor* SkillGroundActor;

	void CheckSkillDirectionPoint();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bShowDebug = true;
};
