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
	CameraBoom->TargetArmLength = 1000.f;
	CameraBoom->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
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

bool AComma::HasTag_FocusedAttack() const
{
	FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(TEXT("Comma.State.IsAttacking"));
	return ASC->HasMatchingGameplayTag(AttackTag);
}

UStaticMeshComponent* AComma::GetSwordWeaponMesh() const
{
	return SwordWeaponMesh;
}

UStaticMeshComponent* AComma::GetBowWeaponMesh() const
{
	return BowWeaponMesh;
}

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
