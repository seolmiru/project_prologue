#include "SplineEffect.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/Character/Player/Comma.h"

ASplineEffect::ASplineEffect()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	ElapsedTime = 0.0f;

	SoulEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SoulEffect"));
	SoulEffectComponent->SetupAttachment(RootComponent);
}

void ASplineEffect::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AComma::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		// 첫 번째 Comma 액터를 TargetActor로 설정
		TargetActor = FoundActors[0];
		UE_LOG(LogTemp, Log, TEXT("TargetActor를 Comma 클래스로 자동 설정했습니다: %s"), *TargetActor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("월드 내에 Comma 클래스 액터가 없습니다."));
		return;
	}
	
	ElapsedTime = 0.0f;

	SoulEffectComponent->Activate(true);
}

void ASplineEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!TargetActor)
		return;

	FVector PrevLocation = GetActorLocation();

	ElapsedTime += DeltaTime;
	float t = ElapsedTime / Duration;
	t = FMath::Clamp(t, 0.f, 1.f);

	FVector Start = StartLocation;
	FVector End = TargetActor->GetActorLocation();

	float NewX = FMath::Lerp(Start.X, End.X, t);
	float NewY = FMath::Lerp(Start.Y, End.Y, t);

	float DeltaZ = End.Z - Start.Z;
	float NewZ = Start.Z + DeltaZ * t + (-4.f * ArcHeight * (t - 0.5f) * (t - 0.5f) + ArcHeight);

	FVector NewLocation(NewX, NewY, NewZ);
	SetActorLocation(NewLocation);
	SoulEffectComponent->SetWorldLocation(NewLocation);

	// 이동 방향 벡터 계산
	FVector Direction = NewLocation - PrevLocation;
	if (!Direction.IsNearlyZero())
	{
		FRotator NewRotation = Direction.Rotation();
		SetActorRotation(NewRotation);
	}

	if (t >= 1.f)
	{
		if (endEventHandler.IsBound())
			endEventHandler.Broadcast(this, this->TargetActor->GetActorLocation());
		
		if(GetSoulEffectTemplate)
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), GetSoulEffectTemplate, this->GetActorLocation());
		
		SoulEffectComponent->Deactivate();
		Destroy();
	}
}

void ASplineEffect::OnStartSpline(FVector StartPosition, AActor* targetActor,
	OnStartSplineEffect StartEventCallbackHandler, OnEndSplineEffecct EndEventCallbackHandler, float PeakHeight)
{
	startEventHandler = StartEventCallbackHandler;
	endEventHandler = EndEventCallbackHandler;

	this->SetActorLocation(StartPosition);
	
	this->StartLocation = StartPosition;
	this->TargetActor = targetActor;
	this->ArcHeight = PeakHeight;

	if (startEventHandler.IsBound())
		startEventHandler.Broadcast(this, this->TargetActor->GetActorLocation());
}



