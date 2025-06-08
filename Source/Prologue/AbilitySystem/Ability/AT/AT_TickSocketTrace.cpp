// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_TickSocketTrace.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemGlobals.h"

UAT_TickSocketTrace* UAT_TickSocketTrace::TickSocketTrace(UGameplayAbility* OwningAbility)
{
	UAT_TickSocketTrace* SocketTrace = NewAbilityTask<UAT_TickSocketTrace>(OwningAbility);

	SocketTrace->TotalDuration = 1.f;
	SocketTrace->StartSocketName = TEXT("TraceSocketStart");
	SocketTrace->EndSocketName = TEXT("TraceSocketEnd");
	SocketTrace->TraceRadius = 250.f;
	SocketTrace->ElapsedTime = 0.f;
	
	return SocketTrace;
}


void UAT_TickSocketTrace::Activate()
{
	Super::Activate();

	HitActors.Empty();
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &UAT_TickSocketTrace::TimerTrace, 0.1f, true);
}

void UAT_TickSocketTrace::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);

	GetWorld()->GetTimerManager().ClearTimer(Timer);
	HitActors.Empty();
}

void UAT_TickSocketTrace::TimerTrace()
{
	ElapsedTime += 0.1f;

	if (Ability)
	{
		AActor* OwnerActor = Ability->GetOwningActorFromActorInfo();

		if (OwnerActor)
		{
			ACharacter* Character = Cast<ACharacter>(OwnerActor);
			if (!Character)
			{
				return;
			}
			
			USkeletalMeshComponent* MeshComp = Character->GetMesh();
			if (!MeshComp)
			{
				return;
			}

			FVector StartLocation = MeshComp->GetSocketLocation(StartSocketName);
			FVector EndLocation = MeshComp->GetSocketLocation(EndSocketName);
			
			TArray<AActor*> IgnoreActors;
			IgnoreActors.Add(OwnerActor);

			TArray<FHitResult> HitResults;

			bool bResult = UKismetSystemLibrary::SphereTraceMulti(
				GetWorld(),
				StartLocation,
				EndLocation,
				TraceRadius,
				UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel3),
				false,
				IgnoreActors,
				bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
				HitResults,
				true,
				FLinearColor::Red,
				FLinearColor::Green,
				0.1f
			);

			if (bResult)
			{
				for (const FHitResult& HitResult : HitResults)
				{
					AActor* HitActor = HitResult.GetActor();

					if (HitActor && !HitActors.Contains(HitActor))
					{
						HitActors.Add(HitActor);

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
						}
					}
				}
			}
		}
	}
}
