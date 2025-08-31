// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerBank.h"

#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/Character/Player/Comma.h"
#include "Prologue/Game/PrologueGameInstance.h"

APowerBank::APowerBank()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(RootComponent);

	PowerBankMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PowerBankMesh"));
	PowerBankMesh->SetupAttachment(RootComponent);

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APowerBank::OnOverlapBegin);

	MaterialTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("MaterialTimeline"));
}

void APowerBank::BeginPlay()
{
	Super::BeginPlay();
	
	if (PowerBankMesh)
	{
		DynamicMaterial = PowerBankMesh->CreateAndSetMaterialInstanceDynamic(0);
	}

	if (TimelineCurve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("TimelineProgress"));
		MaterialTimeline->AddInterpFloat(TimelineCurve, ProgressFunction);
	}

	UPrologueGameInstance* GameInstance = Cast<UPrologueGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance && GameInstance->HasPowerBankInteracted(PowerBankID))
	{
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue(FName("PowerBankSwitch"), 1.f);
			DynamicMaterial->SetScalarParameterValue(FName("Fresnel_Outline"), 0.5f);
			DynamicMaterial->SetScalarParameterValue(FName("Fresnel_OutlineSide"), 1.f);
		}

		TriggerVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void APowerBank::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AComma* Comma = Cast<AComma>(OtherActor);
	if (Comma)
	{
		Comma->GetGuideWidget()->SetVisibility(true);
	}
}

void APowerBank::Interact()
{
	AComma* Comma = Cast<AComma>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
	UPrologueGameInstance* GameInstance = Cast<UPrologueGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance && !GameInstance->HasPowerBankInteracted(PowerBankID))
	{
		if (MaterialTimeline)
		{
			MaterialTimeline->PlayFromStart();
		}

		GameInstance->OnPowerBankActivated(PowerBankID);
		
		TriggerVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		Comma->GetGuideWidget()->SetVisibility(false);
	}
}

void APowerBank::TimelineProgress(float Value)
{
	float SwitchParamValue = FMath::Lerp(0.f, 1.f, Value);
	float OutlineParamValue = FMath::Lerp(0.f, 0.5f, Value);
	float OutlineSideParamValue = FMath::Lerp(0.f, 1.f, Value);

	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(FName("PowerBankSwitch"), SwitchParamValue);
		DynamicMaterial->SetScalarParameterValue(FName("Fresnel_Outline"), OutlineParamValue);
		DynamicMaterial->SetScalarParameterValue(FName("Fresnel_OutlineSide"), OutlineSideParamValue);
	}
}

