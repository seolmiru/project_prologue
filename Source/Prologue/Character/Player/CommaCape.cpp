// Fill out your copyright notice in the Description page of Project Settings.


#include "CommaCape.h"

ACommaCape::ACommaCape()
{
	PrimaryActorTick.bCanEverTick = false;

	CapeMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CapeMesh"));
	SetRootComponent(CapeMesh);
}

