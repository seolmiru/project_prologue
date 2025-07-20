// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueEnemyCharacter.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/AbilitySystem/PrologueAttributeSet.h"
#include "Prologue/Controller/PrologueAIController.h"
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

void APrologueEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (ASC && Attributes)
	{
		DamageAttributeChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(Attributes->GetCurrentHealthAttribute()).AddUObject(this, &APrologueEnemyCharacter::OnDamageAttributeChanged);
	}
}

void APrologueEnemyCharacter::OnDamageAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue > 0.f)
	{
		if (const FGameplayEffectModCallbackData* ModData = Data.GEModData)
		{
			AActor* DamageCauser = ModData->EffectSpec.GetContext().GetEffectCauser();

			if (DamageCauser)
			{
				if (APrologueAIController* AIController = Cast<APrologueAIController>(GetController()))
				{
					if (UBlackboardComponent* BB = AIController->GetBlackboardComponent())
					{
						BB->SetValueAsObject(FName("TargetActor"), DamageCauser);
					}

					AIController->InitiateCombat(DamageCauser);
				}
			}
		}
	}
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

bool APrologueEnemyCharacter::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());

	TArray<FGameplayAbilitySpec*> FoundAbilitySpecs;

	ASC->GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(), FoundAbilitySpecs);

	if (!FoundAbilitySpecs.IsEmpty())
	{
		const int32 RandomAbilityIndex = FMath::RandRange(0, FoundAbilitySpecs.Num() - 1);
		FGameplayAbilitySpec* SpecToActivate = FoundAbilitySpecs[RandomAbilityIndex];

		check(SpecToActivate);

		if (!SpecToActivate->IsActive())
		{
			return ASC->TryActivateAbility(SpecToActivate->Handle);
		}
	}

	return false;
}

bool APrologueEnemyCharacter::TryActivateRandomAbilityWithWeights(const TArray<FWeightedAbilityInfo>& WeightedAbilities)
{
	if (WeightedAbilities.Num() == 0)
	{
		return false;
	}

	float TotalWeight = 0.f;
	for (const FWeightedAbilityInfo& Info : WeightedAbilities)
	{
		if (Info.AbilityTag.IsValid())
		{
			TotalWeight += Info.Weight;
		}
	}

	if (TotalWeight <= 0.f)
	{
		return false;
	}

	float RandomValue = FMath::RandRange(0.f, TotalWeight);

	float CurrentWeight = 0.f;
	for (const FWeightedAbilityInfo& Info : WeightedAbilities)
	{
		if (!Info.AbilityTag.IsValid())
		{
			continue;
		}

		CurrentWeight += Info.Weight;
		if (RandomValue <= CurrentWeight)
		{
			return TryActivateAbilityByTag(Info.AbilityTag);
		}
	}

	return false;
}
