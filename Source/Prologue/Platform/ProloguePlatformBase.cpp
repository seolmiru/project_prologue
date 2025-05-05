// Fill out your copyright notice in the Description page of Project Settings.


#include "ProloguePlatformBase.h"

AProloguePlatformBase::AProloguePlatformBase()
{
	PrimaryActorTick.bCanEverTick = false;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	PlatformMesh->SetupAttachment(RootComponent);
}
