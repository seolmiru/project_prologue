// Fill out your copyright notice in the Description page of Project Settings.


#include "InputBufferComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"

UInputBufferComponent::UInputBufferComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
}

void UInputBufferComponent::BufferInput(FGameplayTag InputTag)
{
	// 입력 버퍼가 가득 찼다면 가장 오래된 입력부터 제거
	if (InputBuffer.Num() >= MaxBufferSize)
	{
		InputBuffer.RemoveAt(0);
	}

	FBufferedInput NewInput;
	NewInput.InputTag = InputTag;
	NewInput.Timestamp = GetWorld()->GetTimeSeconds();

	InputBuffer.Add(NewInput);
}

bool UInputBufferComponent::ConsumeBufferedInput()
{
	if (InputBuffer.Num() == 0)
	{
		return false;
	}

	// 가장 먼저 들어온 입력부터 실행
	FBufferedInput FirstInput = InputBuffer[0];
	InputBuffer.RemoveAt(0);

	ExecuteInput(FirstInput.InputTag);
	return true;
}

bool UInputBufferComponent::HasBufferedInput(FGameplayTag InputTag) const
{
	// 특정 태그의 입력이 버퍼에 존재하는지 확인
	return InputBuffer.ContainsByPredicate([&InputTag](const FBufferedInput& Input)
	{
		return Input.InputTag == InputTag;
	});
}

void UInputBufferComponent::ClearBuffer()
{
	// 입력 버퍼 정리
	InputBuffer.Empty();
}

void UInputBufferComponent::CleanExpiredInputs()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();

	// 오래된 입력은 제거
	InputBuffer.RemoveAll([this, CurrentTime](const FBufferedInput& Input)
	{
		return (CurrentTime - Input.Timestamp) > InputLifeTime;
	});
}

void UInputBufferComponent::ExecuteInput(FGameplayTag InputTag)
{
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Character))
		{
			ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(InputTag));
		}
	}
}


void UInputBufferComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInputBufferComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CleanExpiredInputs();
}

