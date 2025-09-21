// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialGuide.h"

ATutorialGuide::ATutorialGuide()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	GuideWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("GuideWidgetComponent"));
	GuideWidgetComponent->SetupAttachment(RootComponent);

	GuideWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	GuideWidgetComponent->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	GuideWidgetComponent->SetDrawSize(FVector2D(800.f, 600.f));
	GuideWidgetComponent->SetVisibility(true);
}

void ATutorialGuide::BeginPlay()
{
	Super::BeginPlay();

	if (BP_GuideWidget)
	{
		GuideWidgetComponent->SetWidgetClass(BP_GuideWidget);
		ShowGuide();
	}
}

void ATutorialGuide::ShowGuide()
{
	if (GuideWidgetComponent)
	{
		GuideWidgetComponent->SetVisibility(true);
	}
}

void ATutorialGuide::HideGuide()
{
	if (GuideWidgetComponent)
	{
		GuideWidgetComponent->SetVisibility(false);
	}
}

void ATutorialGuide::DestroyGuide()
{
	Destroy();
}

