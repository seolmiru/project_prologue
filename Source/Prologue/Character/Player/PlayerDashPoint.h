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

	/*=======Dash Section=======*/
public:
	// 검사 방향 변경 함수
	void SetDirection(FVector NewDirection);

	FVector GetPoint();
	
private:
	UPROPERTY(VisibleAnywhere, Category="Dash")
	TObjectPtr<class AComma> Player;
	
	// 업데이트 플래그
	UPROPERTY(VisibleAnywhere, Category="Dash")
	bool bTickFlag;

	// 회전 검사 최대 각
	const float RotationMaxDegreeAngle = 15.0f;
	
	// 대쉬 방향
	UPROPERTY(VisibleAnywhere, Category="Dash")	
	FVector TargetDirection;

	FVector CurrentDirection;
	
	// 대쉬 위치
	UPROPERTY(VisibleAnywhere, Category="Dash")	
	FVector Point;

	// 이전 대쉬 대상 지면 액터
	UPROPERTY(VisibleAnywhere, Category="Dash")	
	AActor* GroundActor;
	
	// 최대 거리
	const float MaxDistance = 1000.0f;

	// 대쉬 허용 시야각
	const float FOVAngle = 60.0f;
	
	// 수직 오프셋
	const float VerticalOffset = 500.0f;

	// 검사 단위
	const int32 PartialUnitCount = 64;
	
	// 대쉬 탐색 함수
	void CheckNewDirecionPoint();
	
	// 특정 각도만큼 회전시키는 함수
	FQuat RotateToWorld(const FQuat& From, const FQuat& To, float MaxRadian);
};
