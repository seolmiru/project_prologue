// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeGhost.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Player/Comma.h"

ATimeGhost::ATimeGhost()
{
	PrimaryActorTick.bCanEverTick = false;

	GhostCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("GhostCollision"));
	SetRootComponent(GhostCollision);
	GhostCollision->InitCapsuleSize(42.f, 96.f);
	
	GhostMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GhostMesh"));
	GhostMesh->SetupAttachment(GhostCollision);

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(GhostMesh);
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	TriggerVolume->SetBoxExtent(FVector(50.f, 50.f, 10.f));

	GhostChatBoxWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("GhostChatBoxWidgetComponent"));
	GhostChatBoxWidgetComponent->SetupAttachment(RootComponent);
	GhostChatBoxWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 15.f));
	GhostChatBoxWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	GhostChatBoxWidgetComponent->SetVisibility(false);
}

void ATimeGhost::BeginPlay()
{
	Super::BeginPlay();

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ATimeGhost::OnOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ATimeGhost::OnOverlapEnd);
}

void ATimeGhost::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AComma* Comma = Cast<AComma>(OtherActor))
	{
		GhostChatBoxWidgetComponent->SetVisibility(true);
	}
}

void ATimeGhost::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (AComma* Comma = Cast<AComma>(OtherActor))
	{
		GhostChatBoxWidgetComponent->SetVisibility(false);
	}
}
