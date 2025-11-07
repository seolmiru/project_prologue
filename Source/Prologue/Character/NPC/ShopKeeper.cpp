// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopKeeper.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Prologue/Character/Player/Comma.h"

AShopKeeper::AShopKeeper()
{
	PrimaryActorTick.bCanEverTick = false;
	
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	SetRootComponent(TriggerVolume);
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(TriggerVolume);

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComponent->SetupAttachment(Mesh);

	ShopKeeperWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ShopKeeperWidget"));
	ShopKeeperWidgetComponent->SetupAttachment(Mesh);
	ShopKeeperWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	ShopKeeperWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	ShopKeeperWidgetComponent->SetVisibility(false);

	HealPotionCost = 200.f;
}

void AShopKeeper::BeginPlay()
{
	Super::BeginPlay();

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AShopKeeper::OnOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AShopKeeper::OnOverlapEnd);
}

void AShopKeeper::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AComma* Comma = Cast<AComma>(OtherActor))
	{
		Comma->SetShopKeeper(this);
	}
}

void AShopKeeper::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (AComma* Comma = Cast<AComma>(OtherActor))
	{
		Comma->SetShopKeeper(nullptr);
	}
}

