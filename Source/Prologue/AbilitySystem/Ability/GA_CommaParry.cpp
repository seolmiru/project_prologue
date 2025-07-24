// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CommaParry.h"

#include "AbilitySystemComponent.h"
#include "AT/AT_TickCurve.h"
#include "Kismet/GameplayStatics.h"
#include "Prologue/PrologueGameplayTags.h"
#include "Prologue/Character/Player/Comma.h"
#include "Prologue/Weapon/Projectile/BazierProjectile.h"

void UGA_CommaParry::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AComma* Comma = Cast<AComma>(ActorInfo->AvatarActor.Get());

	Comma->RotateToMouse();
	
	FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle EffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(ParryEffectClass, 0.f, EffectContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	/*UAT_TickCurve* TickCurveTask = UAT_TickCurve::CreateTask(this, Curve);
	TickCurveTask->OnComplete.AddDynamic(this, &UGA_CommaParry::OnComplete);
	TickCurveTask->OnCurveTick.AddDynamic(this, &UGA_CommaParry::OnCurveTick);
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_Parried);

	TickCurveTask->ReadyForActivation();*/

	Deflect(Comma);
}

void UGA_CommaParry::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
}

void UGA_CommaParry::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_CommaParry::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_CommaParry::Deflect(AComma* Comma)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Comma);

	TArray<AActor*> OverlappedActors;
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		Comma->GetActorLocation(),
		ParryRadius,
		ObjectTypes,
		ABazierProjectile::StaticClass(),
		IgnoreActors,
		OverlappedActors
	);

	for (AActor* Actor : OverlappedActors)
	{
		if (ABazierProjectile* Projectile = Cast<ABazierProjectile>(Actor))
		{
			Projectile->Deflected(Comma);

			GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PrologueGameplayTags::GameplayCue_Effect_Parried);
		}
	}
}

/*void UGA_CommaParry::OnCurveTick(float Alpha)
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), Alpha);
}*/
