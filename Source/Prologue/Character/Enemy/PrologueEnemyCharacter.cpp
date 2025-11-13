// Fill out your copyright notice in the Description page of Project Settings.


#include "PrologueEnemyCharacter.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/AbilitySystem/Attribute/PrologueAttributeSet.h"
#include "Prologue/Component/EnemyWidgetComponent.h"
#include "Prologue/Controller/PrologueAIController.h"
#include "Prologue/Game/PrologueGameInstance.h"
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

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	Attributes = CreateDefaultSubobject<UPrologueAttributeSet>(TEXT("Attributes"));

	HpBar = CreateDefaultSubobject<UEnemyWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	HpBar->SetWidgetSpace(EWidgetSpace::Screen);
	HpBar->SetWidgetClass(BP_EnemyWidget);

	ProjectilePool = nullptr;
}

APrologueEnemyCharacter::~APrologueEnemyCharacter()
{
	if (ProjectilePool)
	{
		delete ProjectilePool;
		ProjectilePool = nullptr;
	}
}

void APrologueEnemyCharacter::BeginPlay()
{
	UPrologueGameInstance* GameInstance = Cast<UPrologueGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (GameInstance)
	{
		const FName MyID = GetFName();

		if (GameInstance->HasAIDBeenDestroyed(MyID))
		{
			if (AController* C = GetController())
			{
				C->UnPossess();
				C->Destroy();
			}

			SetActorHiddenInGame(true);

			SetActorEnableCollision(false);

			SetActorTickEnabled(false);
				
			Destroy();
			return;
		}
	}
	
	Super::BeginPlay();

	// 전투 시작 전에는 체력바를 안 보이게 설정
	if (HpBar)
	{
		HpBar->SetVisibility(false);
	}
	
	if (BossHpBarWidget)
	{
		BossHpBarWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	
	if (ASC && Attributes)
	{
		DamageAttributeChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(Attributes->GetCurrentHealthAttribute()).AddUObject(this, &APrologueEnemyCharacter::OnDamageAttributeChanged);
	}

	ProjectilePool = new Pool<AExplodingMangoProjectile>(GetWorld(), MangoProjectileClass, 0);
}

void APrologueEnemyCharacter::OnDamageAttributeChanged(const FOnAttributeChangeData& Data)
{
	// 플레이어에게 공격을 받는 순간 체력바 표시
	if (!bHealthBarVisible && Data.OldValue > Data.NewValue && HpBar)
	{
		HpBar->SetVisibility(true);
		bHealthBarVisible = true;
	}
	
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

	for (auto& Ability : OnGiveAbilities)
	{
		FGameplayAbilitySpec GameplayAbilitySpec(Ability);
		FGameplayAbilitySpecHandle SpecHandle = ASC->GiveAbility(GameplayAbilitySpec);

		ASC->TryActivateAbility(SpecHandle);
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

	if (ASC)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UPrologueAttributeSet::GetCurrentHealthAttribute()).AddUObject(this, &APrologueEnemyCharacter::HealthAttributeChanged);
	}
	
	if (BP_MangoWidget)
	{
		BossHpBarWidget = CreateWidget<UEnemyWidget>(GetWorld(), BP_MangoWidget);
		BossHpBarWidget->SetAbilitySystemComponent(this);
		BossHpBarWidget->AddToViewport();
	}

	if (BP_ChronosWidget)
	{
		BossHpBarWidget = CreateWidget<UEnemyWidget>(GetWorld(), BP_ChronosWidget);\
		BossHpBarWidget->SetAbilitySystemComponent(this);
		BossHpBarWidget->AddToViewport();
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
		if (Info.AbilityTag.IsValid() && Info.AbilityTag != LastUsedAbility)
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
		if (!Info.AbilityTag.IsValid() || Info.AbilityTag == LastUsedAbility)
		{
			continue;
		}

		CurrentWeight += Info.Weight;

		if (RandomValue <= CurrentWeight)
		{
			bool bActivated = TryActivateAbilityByTag(Info.AbilityTag);
			if (bActivated)
			{
				LastUsedAbility = Info.AbilityTag;
			}
			return bActivated;
		}
	}

	return false;
}

void APrologueEnemyCharacter::MarkSelfAsDestroyed()
{
	UPrologueGameInstance* GameInstance = Cast<UPrologueGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (GameInstance)
	{
		const FName MyID = GetFName();

		GameInstance->MarkAIDestroyed(MyID);
	}
}

void APrologueEnemyCharacter::HealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnHealthChanged.Broadcast(Data.OldValue, Data.NewValue);
}
