// Fill out your copyright notice in the Description page of Project Settings.


#include "Comma.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/Controller/CommaController.h"
#include "Prologue/DataAsset/Input/DataAsset_InputConfig.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "PlayerDashPoint.h"
#include "Blueprint/UserWidget.h"
#include "Components/PostProcessComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Prologue/AbilitySystem/Ability/GA_CommaAttackSword.h"
#include "Prologue/Character/ShopKeeper.h"
#include "Prologue/Component/InputBufferComponent.h"
#include "Prologue/PlayerState/ProloguePlayerState.h"
#include "Prologue/UI/Comma/CommaWidget.h"


AComma::AComma()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->SetRelativeRotation(FRotator(-55.f, 0.f, 0.f));
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->FieldOfView = 50.f;
	FollowCamera->bUsePawnControlRotation = false;

	DashCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("DashCollision"));
	DashCollision->SetupAttachment(RootComponent);
	DashCollision->SetActive(false);

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 1000.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 600.f;

	SwordWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordWeaponMesh"));
	SwordWeaponMesh->SetupAttachment(GetMesh(),TEXT("SwordSocket"));

	/*SwordAuraEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SwordAuraEffect"));
	SwordAuraEffect->SetupAttachment(SwordWeaponMesh, FName("AuraSocket"));*/

	UIAnchorComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("UIAnchorComponent"));
	UIAnchorComponent->SetupAttachment(GetRootComponent());
	UIAnchorComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));

	SmashAttackWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("SwitchAttackWidgetComponent"));
	SmashAttackWidgetComponent->SetupAttachment(UIAnchorComponent);
	SmashAttackWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	SmashAttackWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	SmashAttackWidgetComponent->SetDrawSize(FVector2D(400.f, 384.f));
	SmashAttackWidgetComponent->SetVisibility(false);

	CooldownWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("CooldownWidgetComponent"));
	CooldownWidgetComponent->SetupAttachment(RootComponent);
	CooldownWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 15.f));
	CooldownWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

	GuideWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("SpaceBarWidgetComponent"));
	GuideWidgetComponent->SetupAttachment(RootComponent);
	GuideWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	GuideWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	GuideWidgetComponent->SetVisibility(false);

	InputBufferComponent = CreateDefaultSubobject<UInputBufferComponent>(TEXT("InputBufferComponent"));

	SmashAttackSwordTag = FGameplayTag::RequestGameplayTag(FName("Comma.State.SwitchAttack.Sword"));

	SpeedBoostTag = FGameplayTag::RequestGameplayTag(FName("Comma.State.Boost"));

	SwordWeaponMesh->SetVisibility(true);

	/** Sejin */

	// 대쉬 위치 오브젝트 소환
	FActorSpawnParameters SpawnParams;
	UWorld* World = GetWorld();

	static ConstructorHelpers::FClassFinder<APlayerDashPoint> DashRef(
		TEXT("/Script/Engine.Blueprint'/Game/Characters/Comma/Dash/BP_DashPoint.BP_DashPoint_C'"));

	if (World && DashRef.Class)
	{
		DashPoint = World->SpawnActor<APlayerDashPoint>(DashRef.Class, GetActorLocation(), FRotator::ZeroRotator,
		                                                SpawnParams);
	}

	// Pool 테스트
	// TestPool = Pool<APlayerDashPoint>(World, DashRef.Class, 4);
}

void AComma::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FVector Velocity = GetCharacterMovement()->Velocity.GetSafeNormal();
	FVector Forward = GetActorForwardVector();
	FVector Right = GetActorRightVector();

	float ForwardDot = FVector::DotProduct(Velocity, Forward);
	float RightDot = FVector::DotProduct(Velocity, Right);

	Direction = FVector2D(ForwardDot, RightDot);

	if (bIsUsingSmoothRotation && !TargetRotation.IsZero())
	{
		FRotator CurrentRotation = GetActorRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, RotationInterpSpeed);
		SetActorRotation(NewRotation);
	}

	// 카메라 보간
	if (CameraBoom)
	{
		// 회전 보간
		const FRotator CurrentBoomRotation = CameraBoom->GetRelativeRotation();

		const FRotator NewBoomRotation = FMath::RInterpTo(
			CurrentBoomRotation,
			TargetCameraRelativeRotation,
			DeltaSeconds,
			CameraRotationInterpolationSpeed
		);

		CameraBoom->SetRelativeRotation(NewBoomRotation);

		// 거리 보간
		const float CurrentArmLength = CameraBoom->TargetArmLength;

		const float NewArmLength = FMath::FInterpTo(
			CurrentArmLength,
			TargetCameraArmLength,
			DeltaSeconds,
			CameraArmLengthInterpolationSpeed
		);

		CameraBoom->TargetArmLength = NewArmLength;
	}
	
	// 카메라 보정
	if (CameraBoom)
	{
		FVector TargetLocation = GetActorLocation();
		TargetLocation.Z += 100.f;

		FVector CurrentLocation = CameraBoom->GetComponentLocation();
		FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaSeconds, 4.f);
		CameraBoom->SetWorldLocation(NewLocation);
	}

	if (UIAnchorComponent)
	{
		FRotator FixedRotation = FRotator::ZeroRotator;
		UIAnchorComponent->SetWorldRotation(FixedRotation);
	}
}

void AComma::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (ACommaController* CommaController = Cast<ACommaController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(CommaController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AComma::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AComma::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AProloguePlayerState* GASPS = GetPlayerState<AProloguePlayerState>())
	{
		ASC = GASPS->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(GASPS, this);

		// SmashAttackSwordTag, SpeedBoostTag Event 등록
		if (ASC && SmashAttackSwordTag.IsValid())
		{
			ASC->RegisterGameplayTagEvent(SmashAttackSwordTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AComma::OnSmashAttackUI);
		}

		if (ASC && SpeedBoostTag.IsValid())
		{
			ASC->RegisterGameplayTagEvent(SpeedBoostTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AComma::OnDashSpeedBoost);
		}
	}

	if (ASC)
	{
		//ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Comma.Weapon.Hammer")));

		for (auto& Ability : StartAbilities)
		{
			FGameplayAbilitySpec GameplayAbilitySpec(Ability);
			ASC->GiveAbility(GameplayAbilitySpec);
		}

		for (auto& Ability : OnGiveAbilities)
		{
			FGameplayAbilitySpec GameplayAbilitySpec(Ability);
			FGameplayAbilitySpecHandle SpecHandle = ASC->GiveAbility(GameplayAbilitySpec);

			ASC->TryActivateAbility(SpecHandle);
		}
	}

	if (BP_CommaWidget)
	{
		CommaWidget = CreateWidget<UCommaWidget>(GetWorld(), BP_CommaWidget);
		CommaWidget->SetAbilitySystemComponent(this);
		CommaWidget->AddToViewport();
	}

	if (!StartEffect.IsEmpty())
	{
		for (const TSubclassOf<UGameplayEffect>& EffectClass : StartEffect)
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

void AComma::BeginPlay()
{
	Super::BeginPlay();

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp)
	{
		LOG_SCREEN_R("Comma BeginPlay : No CharacterMovementComponent");
		return;
	}

	DefaultWalkSpeed = MoveComp->MaxWalkSpeed;
	
	FGameplayTag DashCooldownTag = FGameplayTag::RequestGameplayTag(FName("Comma.Cooldown.Dash"));
	FDelegateHandle DashCoolHandle = ASC->RegisterGameplayTagEvent(DashCooldownTag, EGameplayTagEventType::NewOrRemoved)
	                                    .AddLambda([this](const FGameplayTag Tag, int32 NewCount)
	                                    {
		                                    if (NewCount == 0 && bInputDash)
		                                    {
			                                    InputGAS(FGameplayTag::RequestGameplayTag(FName("Comma.Ability.Dash")));
		                                    }
	                                    });

	if (ACommaController* CommaController = Cast<ACommaController>(GetController()))
	{
		CommaController->bShowMouseCursor = true;

		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);

		CommaController->SetInputMode(InputMode);
	}

	if (DamagePostProcessMaterial && FollowCamera)
	{
		DamagePostProcessMID = UMaterialInstanceDynamic::Create(DamagePostProcessMaterial, this);
		if (DamagePostProcessMID)
		{
			DamagePostProcessMID->SetScalarParameterValue(FName("DamageIntensity"), 0.f);

			FollowCamera->PostProcessSettings.WeightedBlendables.Array.Add(
				FWeightedBlendable(1.f, DamagePostProcessMID));
		}
	}
}

void AComma::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ASC && SmashAttackSwordTag.IsValid())
	{
		ASC->RegisterGameplayTagEvent(SmashAttackSwordTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	}

	if (ASC && SpeedBoostTag.IsValid())
	{
		ASC->RegisterGameplayTagEvent(SpeedBoostTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void AComma::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AComma::Input_Move);
		FGameplayTag DashTag = FGameplayTag::RequestGameplayTag(FName("Comma.Ability.Dash"));

		if (InputConfigDataAsset)
		{
			for (auto InputAction : InputConfigDataAsset->InputActions)
			{
				EnhancedInputComponent->BindAction(InputAction.InputAction, ETriggerEvent::Started, this,
				                                   &AComma::InputGAS, InputAction.Tag);

				if (InputAction.Tag == DashTag)
				{
					EnhancedInputComponent->BindAction(InputAction.InputAction, ETriggerEvent::Started, this,
					                                   &AComma::InputDash, true);
					EnhancedInputComponent->BindAction(InputAction.InputAction, ETriggerEvent::Completed, this,
					                                   &AComma::InputDash, false);
				}

				LOG_SCREEN("%s", *InputAction.Tag.ToString());
			}
		}
	}
}

void AComma::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	CachedMovementInput = MovementVector;

	/** Sejin */

	// 대쉬 위치 오브젝트에 입력 방향 전달
	if (DashPoint != nullptr)
	{
		DashPoint->SetDirection(FVector(MovementVector.X, MovementVector.Y, 0.f).GetSafeNormal());
	}

	if (ASC)
	{
		if (ASC->HasMatchingGameplayTag(PrologueGameplayTags::Shared_State_IsAttacking))
		{
			return;
		}
	}

	if (Controller != nullptr)
	{
		// get forward vector
		const FVector ForwardDirection = FVector(1.f, 0.f, 0.f);

		// get right vector 
		const FVector RightDirection = FVector(0.f, 1.f, 0.f);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

		if (!MovementVector.IsNearlyZero())
		{
			FVector WorldMovementDirection = ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X;
			WorldMovementDirection.Z = 0.f;
			WorldMovementDirection.Normalize();

			FRotator NewRotation = WorldMovementDirection.Rotation();
			SetActorRotation(NewRotation);
		}
	}
}

void AComma::ActivateRotateCamera(FRotator NewTargetRotation)
{
	TargetCameraRelativeRotation = NewTargetRotation;
}

void AComma::DeactivateRotateCamera()
{
	TargetCameraRelativeRotation = DefaultCameraRelativeRotation;
}

void AComma::ActivateAdjustCamera(float NewTargetArmLength)
{
	TargetCameraArmLength = NewTargetArmLength;
}

void AComma::DeactivateAdjustCamera()
{
	TargetCameraArmLength = DefaultCameraArmLength;
}

UStaticMeshComponent* AComma::GetSwordWeaponMesh() const
{
	return SwordWeaponMesh;
}

void AComma::SetUIVisibility(bool bVisible)
{
	if (CommaWidget)
	{
		ESlateVisibility NewVisibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
		CommaWidget->SetVisibility(NewVisibility);
	}

	if (CooldownWidgetComponent)
	{
		CooldownWidgetComponent->SetVisibility(bVisible);
	}
}

void AComma::HideCommaUI()
{
	if (CommaWidget)
	{
		bCommaWidgetVisibility = (CommaWidget->GetVisibility() == ESlateVisibility::Visible);
		CommaWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	if (CooldownWidgetComponent)
	{
		bCommaWidgetVisibility = CooldownWidgetComponent->IsVisible();
		CooldownWidgetComponent->SetVisibility(false);
	}
}

void AComma::ShowCommaUI()
{
	if (CommaWidget && bCommaWidgetVisibility)
	{
		CommaWidget->SetVisibility(ESlateVisibility::Visible);
	}

	if (CooldownWidgetComponent && bCooldownWidgetVisibility)
	{
		CooldownWidgetComponent->SetVisibility(true);
	}
}

// 활 공격 시에 사용되는 마우스 방향으로 회전하는 함수
void AComma::RotateToMouse()
{
	ACommaController* CommaController = Cast<ACommaController>(GetController());
	if (!CommaController) return;

	float MouseX, MouseY;
	CommaController->GetMousePosition(MouseX, MouseY);

	FVector WorldLocation, WorldDirection;
	CommaController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection);

	FVector MyLocation = GetActorLocation();
	float Z = MyLocation.Z;
	float Distance = (Z - WorldLocation.Z) / WorldDirection.Z;

	FVector Target = WorldLocation + WorldDirection * Distance;
	FVector DirectionToMouse = Target - MyLocation;
	DirectionToMouse.Z = 0;

	if (!DirectionToMouse.IsNearlyZero())
	{
		FRotator NewRotation = DirectionToMouse.Rotation();
		SetActorRotation(NewRotation);
	}
}

// 검 공격 시에 사용되는 마우스 방향으로 회전하는 함수
void AComma::RotateToMouseSmooth()
{
	ACommaController* CommaController = Cast<ACommaController>(GetController());
	if (!CommaController) return;

	float MouseX, MouseY;
	CommaController->GetMousePosition(MouseX, MouseY);

	FVector WorldLocation, WorldDirection;
	CommaController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection);

	FVector MyLocation = GetActorLocation();
	float Z = MyLocation.Z;
	float Distance = (Z - WorldLocation.Z) / WorldDirection.Z;

	FVector Target = WorldLocation + WorldDirection * Distance;
	FVector DirectionToMouse = Target - MyLocation;
	DirectionToMouse.Z = 0;

	if (!DirectionToMouse.IsNearlyZero())
	{
		TargetRotation = DirectionToMouse.Rotation();
		bIsUsingSmoothRotation = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;

		/* Sejin */
		// 공격시 DashPoint도 회전
		DashPoint->SetDirection(DirectionToMouse, false);
	}
}

// TargetLocation으로 회전
void AComma::RotateToTarget(AActor* Target)
{
	if (!Target) return;

	const FVector MyLocation = GetActorLocation();
	const FVector TargetLocation = Target->GetActorLocation();
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(MyLocation, TargetLocation);

	LookAtRot.Pitch = 0.f;
	LookAtRot.Roll = 0.f;

	SetActorRotation(LookAtRot);
}

FVector AComma::GetMouseDirection() const
{
	ACommaController* CommaController = Cast<ACommaController>(GetController());

	float MouseX, MouseY;
	CommaController->GetMousePosition(MouseX, MouseY);

	FVector WorldLocation, WorldDirection;
	CommaController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection);

	FVector MyLocation = GetActorLocation();
	float Z = MyLocation.Z;
	float Distance = (Z - WorldLocation.Z) / WorldDirection.Z;

	FVector Target = WorldLocation + WorldDirection * Distance;
	FVector DirectionToMouse = Target - MyLocation;
	DirectionToMouse.Z = 0;

	if (DirectionToMouse.IsNearlyZero())
	{
		return GetActorForwardVector();
	}

	return DirectionToMouse.GetSafeNormal();
}

void AComma::OnAttackEnded()
{
	bIsUsingSmoothRotation = false;
	TargetRotation = FRotator::ZeroRotator;
}

void AComma::OnSmashAttackUI(const FGameplayTag CallbackTag, int32 NewCount) const
{
	// SmashAttackTag가 들어왔을 때
	if (CallbackTag == SmashAttackSwordTag && SmashAttackWidgetComponent)
	{
		if (NewCount > 0)
		{
			SmashAttackWidgetComponent->SetVisibility(true);
		}
		else
		{
			SmashAttackWidgetComponent->SetVisibility(false);
		}
	}
}

void AComma::TriggerDamageEffect(float DamageAmount)
{
	if (DamageAmount <= 0.f || !DamagePostProcessMID)
	{
		return;
	}

	DamageEffectStartTime = GetWorld()->GetTimeSeconds();

	DamagePostProcessMID->SetScalarParameterValue(FName("DamageIntensity"), DamageEffectIntensity);

	GetWorld()->GetTimerManager().ClearTimer(DamageEffectTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(DamageEffectTimerHandle, this, &AComma::UpdateDamageEffect, 0.05f, true);
}

void AComma::OnDashSpeedBoost(const FGameplayTag CallbackTag, int32 NewCount)
{
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp)
	{
		LOG_SCREEN_R("Comma OnDashSpeedBoost : No CharacterMovementComponent");
		return;
	}

	// SpeedBoostTag가 들어왔을 때
	if (CallbackTag == SpeedBoostTag)
	{
		if (NewCount > 0)
		{
			// MaxWalkSpeed를 SpeedBoost만큼 증가
			MoveComp->MaxWalkSpeed = DefaultWalkSpeed * SpeedBoost;
			LOG_SCREEN("Speed Boost Activated");
		}
		else
		{
			// 태그가 전부 제거되면 MaxWalkSpeed 복구
			MoveComp->MaxWalkSpeed = DefaultWalkSpeed;
			LOG_SCREEN("Speed Boost Deactivated");
		}
	}
}

void AComma::OnInteractShop()
{
	if (!ShopKeeperInRange || !ASC)
	{
		return;
	}

	const UPrologueSkillAttributeSet* SkillAttributeSet = ASC->GetSet<UPrologueSkillAttributeSet>();
	if (!SkillAttributeSet)
	{
		return;
	}

	const float CurrentCurrency = SkillAttributeSet->GetCurrency();
	const float CurrentPotions = SkillAttributeSet->GetCurrentHealPotion();
	const float MaxPotions = SkillAttributeSet->GetMaxHealPotion();

	if (CurrentCurrency >= ShopKeeperInRange->HealPotionCost && CurrentPotions < MaxPotions)
	{
		GetWorld()->GetTimerManager().SetTimer(PurchaseTimerHandle, this, &AComma::PurchaseHealPotion, 3.f, false);

		LOG_SCREEN_R("포션 구매 진행중");
		// UI 추가 예정
	}
	else
	{
		LOG_SCREEN_R("포션 구매 불가. 돈이 부족하거나 포션이 가득 참");
	}
}

void AComma::OnInteractShopCompleted()
{
	GetWorld()->GetTimerManager().ClearTimer(PurchaseTimerHandle);
	// UI 추가 예정
}

void AComma::PurchaseHealPotion()
{
	if (!ShopKeeperInRange || !ASC)
	{
		return;
	}

	if (!ShopKeeperInRange->CostEffect || !ShopKeeperInRange->RewardEffect)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(ShopKeeperInRange);

	FGameplayEffectSpecHandle CostSpecHandle = ASC->MakeOutgoingSpec(ShopKeeperInRange->CostEffect, 1.f, EffectContextHandle);
	if (CostSpecHandle.IsValid())
	{
		ASC->ApplyGameplayEffectSpecToSelf(*CostSpecHandle.Data.Get());
	}

	FGameplayEffectSpecHandle RewardSpecHandle = ASC->MakeOutgoingSpec(ShopKeeperInRange->RewardEffect, 1.f, EffectContextHandle);
	if (RewardSpecHandle.IsValid())
	{
		ASC->ApplyGameplayEffectSpecToSelf(*RewardSpecHandle.Data.Get());
	}

	LOG_SCREEN_R("포션 구매 완료");
}

void AComma::SetShopKeeper(AShopKeeper* ShopKeeper)
{
	ShopKeeperInRange = ShopKeeper;

	if (!ShopKeeperInRange)
	{
		GetWorld()->GetTimerManager().ClearTimer(PurchaseTimerHandle);

		// UI 추가 예정
	}
}

void AComma::UpdateDamageEffect()
{
	if (!DamagePostProcessMID)
		return;

	float CurrentTime = GetWorld()->GetTimeSeconds();
	float ElapsedTime = CurrentTime - DamageEffectStartTime;
	float Alpha = ElapsedTime / DamageEffectDuration;

	if (Alpha >= 5.f)
	{
		DamagePostProcessMID->SetScalarParameterValue(FName("DamageIntensity"), 0.f);
		GetWorld()->GetTimerManager().ClearTimer(DamageEffectTimerHandle);
		return;
	}

	float CurrentIntensity = DamageEffectIntensity * (5.f - Alpha);
	DamagePostProcessMID->SetScalarParameterValue(FName("DamageIntensity"), CurrentIntensity);
}

APlayerDashPoint* AComma::GetDashPoint() const
{
	if (DashPoint != nullptr)
	{
		return DashPoint;
	}
	else
	{
		return nullptr;
	}
}

AActor* AComma::GetGround() const
{
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	FVector MyLocation = GetActorLocation();

	FVector FloorStart = MyLocation;
	FVector FloorEnd = MyLocation;
	FloorEnd.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 50.0f;
	const float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	const float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	bool bPlayerHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		MyLocation,
		FloorEnd,
		FQuat::Identity,
		ECC_GameTraceChannel8,
		FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight),
		Params
	);

	if (bPlayerHit)
	{
		return HitResult.GetActor();
	}

	return nullptr;
}

void AComma::InputDash(bool bInput)
{
	// UE_LOG(LogTemp, Log, TEXT("Input Dash Event: %s"), *LexToString(bInput));
	bInputDash = bInput;
}

bool AComma::GetInputDashState() const
{
	return bInputDash;
}
