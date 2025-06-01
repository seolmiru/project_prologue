// Fill out your copyright notice in the Description page of Project Settings.


#include "Comma.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/Component/Combat/CommaCombatComponent.h"
#include "Prologue/Controller/CommaController.h"
#include "Prologue/DataAsset/Input/DataAsset_InputConfig.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Prologue/Player/ProloguePlayerState.h"
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

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	SwordWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordWeaponMesh"));
	SwordWeaponMesh->SetupAttachment(GetMesh(),TEXT("SwordSocket"));

	BowWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BowWeaponMesh"));
	BowWeaponMesh->SetupAttachment(GetMesh(),TEXT("BowSocket"));
	
	CommaCombatComponent = CreateDefaultSubobject<UCommaCombatComponent>(TEXT("CommaCombatComponent"));

	UIAnchorComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("UIAnchorComponent"));
	UIAnchorComponent->SetupAttachment(GetRootComponent());
	UIAnchorComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	
	SwitchAttackWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("SwitchAttackWidgetComponent"));
	SwitchAttackWidgetComponent->SetupAttachment(UIAnchorComponent);
	SwitchAttackWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	SwitchAttackWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	SwitchAttackWidgetComponent->SetDrawSize(FVector2D(400.f, 384.f));
	SwitchAttackWidgetComponent->SetVisibility(false);

	SwitchAttackSwordTag = FGameplayTag::RequestGameplayTag(FName("Comma.State.SwitchAttack.Sword"));

	SwitchAttackBowTag = FGameplayTag::RequestGameplayTag(FName("Comma.State.SwitchAttack.Bow"));
	
	SwordWeaponMesh->SetVisibility(false);
}

UPawnCombatComponent* AComma::GetPawnCombatComponent() const
{
	return CommaCombatComponent;
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

	if (CameraBoom)
	{
		FVector TargetLocation = GetActorLocation();
		TargetLocation.Z += 100.f;

		FVector CurrentLocation = CameraBoom->GetComponentLocation();
		FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaSeconds, 4.f);
		CameraBoom->SetWorldLocation(NewLocation);
	}

	if (bIsUsingSmoothRotation && !TargetRotation.IsZero())
	{
		FRotator CurrentRotation = GetActorRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, RotationInterpSpeed);
		SetActorRotation(NewRotation);
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
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(CommaController->GetLocalPlayer()))
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

		if (ASC && SwitchAttackSwordTag.IsValid())
		{
			ASC->RegisterGameplayTagEvent(SwitchAttackSwordTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AComma::OnSwitchAttackUI);
		}

		if (ASC && SwitchAttackBowTag.IsValid())
		{
			ASC->RegisterGameplayTagEvent(SwitchAttackBowTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AComma::OnSwitchAttackUI);
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
	}

	if (BP_CommaWidget)
	{
		CommaWidget = CreateWidget<UCommaWidget>(GetWorld(), BP_CommaWidget);
		CommaWidget->SetAbilitySystemComponent(this);
		CommaWidget->AddToViewport();
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

void AComma::BeginPlay()
{
	Super::BeginPlay();

	if (ACommaController* CommaController = Cast<ACommaController>(GetController()))
	{
		CommaController->bShowMouseCursor = true;

		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);

		CommaController->SetInputMode(InputMode);
	}
}

void AComma::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ASC && SwitchAttackSwordTag.IsValid())
	{
		ASC->RegisterGameplayTagEvent(SwitchAttackSwordTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	}
	
	if (ASC && SwitchAttackBowTag.IsValid())
	{
		ASC->RegisterGameplayTagEvent(SwitchAttackBowTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void AComma::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AComma::Input_Move);

		if (InputConfigDataAsset)
		{
			for (auto InputAction : InputConfigDataAsset->InputActions)
			{
				EnhancedInputComponent->BindAction(InputAction.InputAction, ETriggerEvent::Started, this, &AComma::InputGAS, InputAction.Tag);

				LOG_SCREEN("%s", *InputAction.Tag.ToString());
			}
		}
	}
}

void AComma::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	CachedMovementInput = MovementVector;
	
	if (ASC)
	{
		if (ASC->HasMatchingGameplayTag(PrologueGameplayTags::Shared_State_IsAttacking))
		{
			return;
		}
	}

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

UStaticMeshComponent* AComma::GetSwordWeaponMesh() const
{
	return SwordWeaponMesh;
}

UStaticMeshComponent* AComma::GetBowWeaponMesh() const
{
	return BowWeaponMesh;
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

void AComma::OnAttackEnded()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bIsUsingSmoothRotation = false;
	TargetRotation = FRotator::ZeroRotator;
}

void AComma::OnSwitchAttackUI(const FGameplayTag CallbackTag, int32 NewCount) const
{
	if (CallbackTag == SwitchAttackSwordTag && SwitchAttackWidgetComponent)
	{
		if (NewCount > 0)
		{
			SwitchAttackWidgetComponent->SetVisibility(true);
		}
		else
		{
			SwitchAttackWidgetComponent->SetVisibility(false);
		}
	}

	if (CallbackTag == SwitchAttackBowTag && SwitchAttackWidgetComponent)
	{
		if (NewCount > 0)
		{
			SwitchAttackWidgetComponent->SetVisibility(true);
		}
		else
		{
			SwitchAttackWidgetComponent->SetVisibility(false);
		}
	}
}
