// Fill out your copyright notice in the Description page of Project Settings.


#include "Comma.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/AbilitySystem/PrologueAbilitySystemComponent.h"
#include "Prologue/Component/PrologueInputComponent.h"
#include "Prologue/Component/Combat/CommaCombatComponent.h"
#include "Prologue/DataAsset/DataAsset_StartUpDataBase.h"
#include "Prologue/DataAsset/Input/DataAsset_InputConfig.h"


AComma::AComma()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 1000.f;
	CameraBoom->SetRelativeRotation(FRotator(-50.f, -45.f, 0.f));
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bDoCollisionTest = false;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	
	CommaCombatComponent = CreateDefaultSubobject<UCommaCombatComponent>(TEXT("CommaCombatComponent"));
}

UPawnCombatComponent* AComma::GetPawnCombatComponent() const
{
	return CommaCombatComponent;
}

void AComma::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!CharacterStartUpData.IsNull())
	{
		if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
		{
			LoadedData->GiveToAbilitySystemComponent(PrologueAbilitySystemComponent);
		}
	}
}

void AComma::BeginPlay()
{
	Super::BeginPlay();
}

void AComma::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	checkf(InputConfigDataAsset, TEXT("Forgot to assign a valid data asset as input config"));

	ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem);

	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);

	UPrologueInputComponent* PrologueInputComponent = CastChecked<UPrologueInputComponent>(PlayerInputComponent);

	PrologueInputComponent->BindNativeInputAction(InputConfigDataAsset, PrologueGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);

	PrologueInputComponent->BindAbilityInputAction(InputConfigDataAsset, this, &ThisClass::Input_AbilityInputPressed, &ThisClass::Input_AbilityInputReleased);
}

void AComma::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();

	if (MovementVector.Y != 0.f)
	{
		AddMovementInput(FVector::ForwardVector, MovementVector.Y);
	}

	if (MovementVector.X != 0.f)
	{
		AddMovementInput(FVector::RightVector, MovementVector.X);
	}
}

void AComma::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	PrologueAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
}

void AComma::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
	PrologueAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
}
