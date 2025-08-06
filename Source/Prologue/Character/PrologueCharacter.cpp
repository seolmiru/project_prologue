// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrologueCharacter.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Prologue/PrologueGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "MotionWarpingComponent.h"
#include "Prologue/Component/InputBufferComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// APrologueCharacter

APrologueCharacter::APrologueCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

	GetMesh()->bReceivesDecals = false;

	GetCharacterMovement()->bCanWalkOffLedges = false;
	GetCharacterMovement()->PerchRadiusThreshold = 100.f;
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;
}

UPawnCombatComponent* APrologueCharacter::GetPawnCombatComponent() const
{
	return nullptr;
}

void APrologueCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void APrologueCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (ASC && IsValid(this))
	{
		// 태그 수신 대기
		ToughnessTagHandle = ASC->RegisterGameplayTagEvent(PrologueGameplayTags::Shared_State_IsOutOfToughness, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &APrologueCharacter::OnToughnessTagChanged);
	}
}

void APrologueCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ASC && ToughnessTagHandle.IsValid())
	{
		ASC->UnregisterGameplayTagEvent(ToughnessTagHandle, PrologueGameplayTags::Shared_State_IsOutOfToughness, EGameplayTagEventType::NewOrRemoved);
		ToughnessTagHandle.Reset();
	}
	
	Super::EndPlay(EndPlayReason);
}

// 강인도가 0이 되었을 때 실행되는 함수
void APrologueCharacter::OnToughnessTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		LOG_SCREEN_R("Toughness Broken, Applying HitReact");

		if (HitReactEffectClass && ASC)
		{
			// HitReactEffectClass를 통해서 HitReact Ability를 Trigger시킴
			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			ContextHandle.AddSourceObject(this);

			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(HitReactEffectClass, 1.0f, ContextHandle);
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}

		// 1초의 딜레이를 주고 Shared_State_IsOutOfToughness 태그 제거
		GetWorld()->GetTimerManager().SetTimer(ToughnessRecoveryTimerHandle, this, &APrologueCharacter::RecoverToughness, 1.0f, false);
	}
}

void APrologueCharacter::RecoverToughness()
{
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(PrologueGameplayTags::Shared_State_IsOutOfToughness);
	}
}

void APrologueCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APrologueCharacter::InputGAS(const FGameplayTag Tag)
{
	FGameplayTagContainer GameplayTags;

	LOG_SCREEN("%s", *Tag.ToString());
	
	GameplayTags.AddTag(Tag);
	if (ASC)
	{
		// 캔슬 가능 어빌리티
		static TArray<FGameplayTag> CancelableTags = {
			FGameplayTag::RequestGameplayTag(FName("Comma.Ability.Dash")),
			FGameplayTag::RequestGameplayTag(FName("Comma.Ability.Skill"))
		};

		// 선입력에서 제외시킬 어빌리티
		static TArray<FGameplayTag> NonBufferTags = {
			FGameplayTag::RequestGameplayTag(FName("Comma.Ability.Attack.Sword")),
			FGameplayTag::RequestGameplayTag(FName("Comma.Ability.Attack.Bow"))
		};

		bool bShouldBuffer = !NonBufferTags.Contains(Tag);

		bool bCanCancel = CancelableTags.Contains(Tag) && ASC->HasMatchingGameplayTag(PrologueGameplayTags::Comma_State_CancelEnabled);

		// NonBufferTags이고 캔슬 가능한 어빌리티가 아닐 때에만 선입력 배열에 저장
		if (bShouldBuffer && ASC->HasMatchingGameplayTag(PrologueGameplayTags::Shared_State_IsAttacking) && !bCanCancel)
		{
			InputBufferComponent->BufferInput(Tag);
			return;
		}
		
		TArray<FGameplayAbilitySpec*> AbilitiesToActivatePtrs;
		ASC->GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTags, AbilitiesToActivatePtrs);
		if (AbilitiesToActivatePtrs.Num() < 1)
		{
			LOG_SCREEN("No Ability Specifications Found");
			return;
		}
		
		TArray<FGameplayAbilitySpec> AbilitiesToActivate;
		AbilitiesToActivate.Reserve(AbilitiesToActivatePtrs.Num());
		Algo::Transform(AbilitiesToActivatePtrs, AbilitiesToActivate, [](FGameplayAbilitySpec* SpecPtr) { return *SpecPtr; });
		
		for (FGameplayAbilitySpec& GameplayAbilitySpec : AbilitiesToActivate)
		{
			ensure(IsValid(GameplayAbilitySpec.Ability));
			if (GameplayAbilitySpec.IsActive())
			{
				ASC->AbilitySpecInputPressed(GameplayAbilitySpec);
			}
			else
			{
				ASC->TryActivateAbility(GameplayAbilitySpec.Handle);
			}
		}
	}
}

UAbilitySystemComponent* APrologueCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}
