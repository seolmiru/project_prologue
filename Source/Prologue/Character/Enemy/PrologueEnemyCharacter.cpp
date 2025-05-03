// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueEnemyCharacter.h"

#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Prologue/AbilitySystem/PrologueAttributeSet.h"
#include "Prologue/UI/Enemy/EnemyWidget.h"

APrologueEnemyCharacter::APrologueEnemyCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 180.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 250.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	Attributes = CreateDefaultSubobject<UPrologueAttributeSet>(TEXT("Attributes"));

	EnemyWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyWidgetComponent"));
	EnemyWidgetComponent->SetupAttachment(GetMesh());
	EnemyWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	EnemyWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	EnemyWidgetComponent->SetDrawSize(FVector2D(200.f, 50.f));
	EnemyWidgetComponent->SetWidgetClass(BP_EnemyWidget);
}

void APrologueEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ASC->InitAbilityActorInfo(this, this);

	for (auto Ability : StartAbilities)
	{
		ASC->GiveAbility(Ability);
	}

	if (!StartEffect.IsEmpty())
	{
		for (const TSubclassOf<UGameplayEffect> &EffectClass : StartEffect)
		{
			if (!EffectClass) continue;

			UGameplayEffect* EffectCDO = EffectClass->GetDefaultObject<UGameplayEffect>();
			ASC->ApplyGameplayEffectToSelf(
				EffectCDO,
				1.0f,
				ASC->MakeEffectContext()
			);
		}
	}
}
