// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_TickBoxTrace.h"

#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemGlobals.h"

UAT_TickBoxTrace* UAT_TickBoxTrace::TickBoxTrace(UGameplayAbility* OwningAbility)
{
	UAT_TickBoxTrace* BoxTrace = NewAbilityTask<UAT_TickBoxTrace>(OwningAbility);

	BoxTrace->TotalDuration = 1.f;
	BoxTrace->BoxHalfSize = FVector(50.f, 50.f, 50.f);
	BoxTrace->TraceLength = 800.f;
	BoxTrace->DamageInterval = 0.3f;
	BoxTrace->ElapsedTime = 0.f;

	return BoxTrace;
}


void UAT_TickBoxTrace::Activate()
{
	Super::Activate();

	GetWorld()->GetTimerManager().SetTimer(Timer, this, &UAT_TickBoxTrace::TimerTrace, 0.1f, true);
}

void UAT_TickBoxTrace::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);

	GetWorld()->GetTimerManager().ClearTimer(Timer);
	TimeMap.Empty();
}

void UAT_TickBoxTrace::TimerTrace()
{
	ElapsedTime += 0.1f;

	if (ElapsedTime > TotalDuration)
	{
		EndTask();
		return;
	}

	if (Ability)
	{
		AActor* OwnerActor = Ability->GetOwningActorFromActorInfo();
		if (OwnerActor)
		{
			FVector StartLocation = OwnerActor->GetActorLocation();
			FVector ForwardVector = OwnerActor->GetActorForwardVector();
			FVector EndLocation = StartLocation + (ForwardVector * TraceLength);

			TArray<AActor*> IgnoreActors;
			IgnoreActors.Add(OwnerActor);

			TArray<FHitResult> HitResults;

			bool bResult = UKismetSystemLibrary::BoxTraceMulti(
				OwnerActor->GetWorld(),
				StartLocation,
				EndLocation,
				BoxHalfSize,
				OwnerActor->GetActorRotation(),
				UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel3),
				false,
				IgnoreActors,
				bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
				HitResults,
				true
			);

			// BoxTrace에 감지된 대상에게 일정 시간마다 대미지 적용
			if (bResult)
			{
				float CurrentTime = GetWorld()->GetTimeSeconds();

				for (const FHitResult& HitResult : HitResults)
				{
					AActor* HitActor = HitResult.GetActor();
					if (!HitActor) continue;

					// 마지막으로 피격당한 다음 DamageInterval만큼 지났는지 확인
					float* LastDamageTime = TimeMap.Find(HitActor);
					if (LastDamageTime && (CurrentTime - *LastDamageTime) < DamageInterval)
					{
						continue;
					}

					UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor);

					if (TargetASC && AbilitySystemComponent.IsValid())
					{
						FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
						EffectContextHandle.AddSourceObject(OwnerActor);

						float EffectLevel = Ability->GetAbilityLevel(Ability->GetCurrentAbilitySpecHandle(), Ability->GetCurrentActorInfo());
						FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DamageEffectClass, EffectLevel, EffectContextHandle);
						FGameplayAbilityTargetDataHandle DataHandle;
						FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
						DataHandle.Add(TargetData);

						if (OnTraceResultCallback.IsBound())
						{
							OnTraceResultCallback.Broadcast(DataHandle);
						}

						// 감지된 액터가 마지막으로 대미지를 입은 시간 추적
						TimeMap.Add(HitActor, CurrentTime);
					}
				}
			}
		}
	}
}
