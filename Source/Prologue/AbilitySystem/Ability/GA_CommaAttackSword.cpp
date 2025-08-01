// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CommaAttackSword.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/Character/Player/Comma.h"
#include "Prologue/DataAsset/ComboSwordData.h"
#include "Prologue/Character/Enemy/PrologueEnemyCharacter.h"
#include "AT/AT_MoveToTarget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Prologue/Prologue.h"
#include "DrawDebugHelpers.h"
#include "Prologue/Controller/CommaController.h"

UGA_CommaAttackSword::UGA_CommaAttackSword()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_CommaAttackSword::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return !ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Comma.State.SwitchAttack.Sword")));
}

void UGA_CommaAttackSword::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CachedComma = CastChecked<AComma>(ActorInfo->AvatarActor.Get());
	
	CurrentComboData = CachedComma->GetComboSwordData();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->AddLooseGameplayTag(PrologueGameplayTags::Comma_State_CancelDisabled);
		ASC->RemoveLooseGameplayTag(PrologueGameplayTags::Comma_State_CancelEnabled);
	}
	
	if (CurrentComboData && CurrentCombo >= CurrentComboData->MaxComboCount)
	{
		CurrentCombo = 0;
		LOG_SCREEN_R("AttackSword : Reset Combo Count");
	}

	CachedComma->CurrentSwordCombo = CurrentCombo;
	
	if (CurrentCombo == 0)
	{
		CachedAttackDirection = GetMouseDirection();
		
		if (AActor* TargetEnemy = FindNearestEnemyInDirection(CachedAttackDirection))
		{
			CachedComma->RotateToTarget(TargetEnemy);
			
			// 감지된 적과 플레이어 캐릭터의 거리 계산
			FVector Direction = (TargetEnemy->GetActorLocation() - CachedComma->GetActorLocation()).GetSafeNormal();
			FVector TargetLocation = TargetEnemy->GetActorLocation() - (Direction * DashDistance);
			
			float AdjustedDashDuration = DashDuration;

			// 추격 시스템 태스크 실행
			UAT_MoveToTarget* MoveTask = UAT_MoveToTarget::MoveToTarget(this, TargetLocation, AdjustedDashDuration);
			MoveTask->OnFinished.AddDynamic(this, &UGA_CommaAttackSword::OnMoveToTargetFinished);
			MoveTask->ReadyForActivation();
			
			return;
		}
		else
		{
			// 적이 없으면 마우스 방향으로 회전
			CachedComma->RotateToMouseSmooth();
		}
	}
	else
	{
		// 콤보 공격은 마우스 방향으로 회전
		CachedComma->RotateToMouseSmooth();
	}
	
	// 감지된 적이 없거나 콤보 진행 중일 때는 바로 공격 진행
	StartAttackMontage();
}

void UGA_CommaAttackSword::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if (!ComboTimerHandle.IsValid())
	{
		ProcessNextCombo();
	}
	else
	{
		HasNextComboInput = true;
	}
}

void UGA_CommaAttackSword::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
	CurrentComboData = nullptr;
	HasNextComboInput = false;
}

void UGA_CommaAttackSword::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UAbilitySystemComponent* ASC =GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(PrologueGameplayTags::Comma_State_CancelDisabled);
		ASC->RemoveLooseGameplayTag(PrologueGameplayTags::Comma_State_CancelEnabled);
	}
	
	GetWorld()->GetTimerManager().SetTimer(CurrentComboTimerHandle, this, &UGA_CommaAttackSword::ResetComboCount, 0.6f, false);

	if (CachedComma)
	{
		CachedComma->OnAttackEnded();
	}
	
	CurrentComboData = nullptr;
	HasNextComboInput = false;
	CachedComma = nullptr;
}

void UGA_CommaAttackSword::OnComplete()
{
	if (!HasNextComboInput && !GetWorld()->GetTimerManager().IsTimerActive(ComboTimerHandle))
	{
		bool bReplicatedEndAbility = true;
		bool bWasCancelled = false;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
	}
}

void UGA_CommaAttackSword::OnInterrupted()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UGA_CommaAttackSword::OnBlendOut()
{
	if (!HasNextComboInput && !GetWorld()->GetTimerManager().IsTimerActive(ComboTimerHandle))
	{
		bool bReplicatedEndAbility = true;
		bool bWasCancelled = false;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
	}
}

void UGA_CommaAttackSword::OnMoveToTargetFinished()
{
	// 이동 완료 후 공격 시작
	StartAttackMontage();
}

FName UGA_CommaAttackSword::GetNextSection()
{
	LOG_SCREEN_R("GetNextSection - Before : %d", CurrentCombo);
	
	CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, CurrentComboData->MaxComboCount);

	if (CachedComma)
	{
		CachedComma->CurrentSwordCombo = CurrentCombo;
	}
	
	FName NextSection = *FString::Printf(TEXT("%s%d"), *CurrentComboData->MontageSectionNamePrefix, CurrentCombo);
	return NextSection;
}

void UGA_CommaAttackSword::StartComboTimer()
{
	int32 ComboIndex = CurrentCombo - 1;
	ensure(CurrentComboData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	const float ComboEffectiveTime = CurrentComboData->EffectiveFrameCount[ComboIndex] / CurrentComboData->FrameRate;

	if (ComboEffectiveTime > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &UGA_CommaAttackSword::CheckComboInput, ComboEffectiveTime, false);
	}
	else
	{
		EnableComboInput();
	}
}

void UGA_CommaAttackSword::CheckComboInput()
{
	ComboTimerHandle.Invalidate();
	if (HasNextComboInput)
	{
		// 최대 콤보 수 도달 시에는 콤보 진행 제어
		if (CurrentCombo >= CurrentComboData->MaxComboCount)
		{
			HasNextComboInput = false;
			return;
		}
		
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		if (ASC)
		{
			ASC->AddLooseGameplayTag(PrologueGameplayTags::Comma_State_CancelDisabled);
			ASC->RemoveLooseGameplayTag(PrologueGameplayTags::Comma_State_CancelEnabled);
		}

		if (CachedComma)
		{
			CachedComma->RotateToMouseSmooth();
		}
		
		MontageJumpToSection(GetNextSection());
		StartComboTimer();
		HasNextComboInput = false;
	}
}

void UGA_CommaAttackSword::ResetComboCount()
{
	CurrentCombo = 0;
	LOG_SCREEN_R("AttackSword : Reset Combo Count");
}

void UGA_CommaAttackSword::EnableComboInput()
{
	ComboTimerHandle.Invalidate();

	if (HasNextComboInput)
	{
		ProcessNextCombo();
	}
}

void UGA_CommaAttackSword::ProcessNextCombo()
{
	LOG_SCREEN_R("ProcessNextCombo - Current : %d", CurrentCombo);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->AddLooseGameplayTag(PrologueGameplayTags::Comma_State_CancelDisabled);
		ASC->RemoveLooseGameplayTag(PrologueGameplayTags::Comma_State_CancelEnabled);
	}
	
	if (CurrentCombo >= CurrentComboData->MaxComboCount)
	{
		return;
	}

	if (CachedComma)
	{
		CachedComma->RotateToMouseSmooth();
	}
	
	MontageJumpToSection(GetNextSection());
	StartComboTimer();
	HasNextComboInput = false;
}

// 추격 시스템의 감지 방향과 마우스 클릭 방향을 동기화 시키기 위한 함수
FVector UGA_CommaAttackSword::GetMouseDirection() const
{
	if (!CachedComma)
	{
		return FVector::ForwardVector;
	}

	ACommaController* CommaController = Cast<ACommaController>(CachedComma->GetController());
	if (!CommaController)
	{
		return CachedComma->GetActorForwardVector();
	}

	float MouseX, MouseY;
	CommaController->GetMousePosition(MouseX, MouseY);

	FVector WorldLocation, WorldDirection;
	CommaController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection);

	FVector MyLocation = CachedComma->GetActorLocation();
	float Z = MyLocation.Z;
	float Distance = (Z - WorldLocation.Z) / WorldDirection.Z;

	FVector Target = WorldLocation + WorldDirection * Distance;
	FVector DirectionToMouse = Target - MyLocation;
	DirectionToMouse.Z = 0;
	DirectionToMouse.Normalize();

	return DirectionToMouse;
}

AActor* UGA_CommaAttackSword::FindNearestEnemyInDirection(const FVector& SearchDirection)
{
	if (!CachedComma)
	{
		return nullptr;
	}

	const FVector CommaLocation = CachedComma->GetActorLocation();

	// 탐지 범위 디버그
	if (bShowDebugDetection)
	{
		DrawDebugSphere(
			GetWorld(),
			CommaLocation,
			DetectionRange,
			32,
			FColor::Yellow,
			false,
			2.f,
			0,
			2.0f
		);

		// 탐지 각도 디버그
		const float HalfAngleDeg = DetectionAngle * 0.5f;
		const int32 NumSegments = 16;
		
		for (int32 i = 0; i <= NumSegments; ++i)
		{
			float CurrentAngle = -HalfAngleDeg + (HalfAngleDeg * 2.0f * i / NumSegments);
			FVector Direction = SearchDirection.RotateAngleAxis(CurrentAngle, FVector::UpVector);
			FVector EndPoint = CommaLocation + Direction * DetectionRange;
			
			DrawDebugLine(
				GetWorld(),
				CommaLocation,
				EndPoint,
				FColor::Cyan,
				false,
				2.f,
				0,
				2.0f
			);

			if (i > 0)
			{
				float PrevAngle = -HalfAngleDeg + (HalfAngleDeg * 2.0f * (i - 1) / NumSegments);
				FVector PrevDirection = SearchDirection.RotateAngleAxis(PrevAngle, FVector::UpVector);
				FVector PrevEndPoint = CommaLocation + PrevDirection * DetectionRange;
				
				DrawDebugLine(
					GetWorld(),
					PrevEndPoint,
					EndPoint,
					FColor::Cyan,
					false,
					2.f,
					0,
					2.0f
				);
			}
		}
	}

	// 범위 내의 모든 적 감지
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(CachedComma);

	TArray<AActor*> OverlappedActors;
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		CommaLocation,
		DetectionRange,
		ObjectTypes,
		APrologueEnemyCharacter::StaticClass(),
		IgnoreActors,
		OverlappedActors
	);

	// 캐릭터 전방 범위 내에 가장 가까운 적 찾기
	AActor* NearestEnemy = nullptr;
	float NearestDistance = TNumericLimits<float>::Max();
	
	const float HalfAngleRad = FMath::DegreesToRadians(DetectionAngle * 0.5f);

	for (AActor* Actor : OverlappedActors)
	{
		FVector ToEnemy = (Actor->GetActorLocation() - CommaLocation).GetSafeNormal();
		float DotProduct = FVector::DotProduct(SearchDirection, ToEnemy);
		float AngleRad = FMath::Acos(FMath::Clamp(DotProduct, -1.0f, 1.0f));

		// 캐릭터 전방 기준 90도 내에 적이 있는지 확인
		if (AngleRad <= HalfAngleRad)
		{
			float Distance = FVector::Dist(Actor->GetActorLocation(), CommaLocation);
			if (Distance < NearestDistance)
			{
				NearestDistance = Distance;
				NearestEnemy = Actor;
			}

			// 탐지된 적 표시 디버그
			if (bShowDebugDetection)
			{
				DrawDebugSphere(
					GetWorld(),
					Actor->GetActorLocation(),
					50.0f,
					12,
					FColor::Red,
					false,
					2.f,
					0,
					3.0f
				);

				// 캐릭터랑 감지된 적까지 이어주는 선
				DrawDebugLine(
					GetWorld(),
					CommaLocation,
					Actor->GetActorLocation(),
					FColor::Orange,
					false,
					2.f,
					0,
					2.0f
				);
			}
		}
	}

	// 돌진 대상 강조 표시 디버그
	if (NearestEnemy && bShowDebugDetection)
	{
		DrawDebugSphere(
			GetWorld(),
			NearestEnemy->GetActorLocation(),
			60.0f,
			16,
			FColor::Magenta,
			false,
			2.f,
			0,
			5.0f
		);

		// 타겟 표시
		DrawDebugBox(
			GetWorld(),
			NearestEnemy->GetActorLocation() + FVector(0, 0, 100),
			FVector(20, 20, 20),
			FColor::Magenta,
			false,
			2.f,
			0,
			3.0f
		);
	}

	return NearestEnemy;
}

AActor* UGA_CommaAttackSword::FindNearestEnemyInFront()
{
	return FindNearestEnemyInDirection(CachedComma->GetActorForwardVector());
}

void UGA_CommaAttackSword::StartAttackMontage()
{
	if (!CachedComma)
	{
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,  TEXT("PlayAttack"), CachedComma->GetSwordComboMontage(), 1.0f, GetNextSection()
	);
	
	PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_CommaAttackSword::OnComplete);
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_CommaAttackSword::OnInterrupted);
	PlayAttackTask->OnBlendOut.AddDynamic(this, &UGA_CommaAttackSword::OnBlendOut);
	PlayAttackTask->ReadyForActivation();
	
	GetWorld()->GetTimerManager().ClearTimer(CurrentComboTimerHandle);
	
	StartComboTimer();
}