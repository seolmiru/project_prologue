// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_PlaySound3D.h"

#include "Kismet/GameplayStatics.h"

void UAnimNotify_PlaySound3D::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!Sound || !MeshComp)
	{
		return;
	}

	FVector Location;

	// 소켓 지정 시에는 소켓 위치, 아닐 시에는 컴포넌트 위치 사용
	if (AttachName != NAME_None && MeshComp->DoesSocketExist(AttachName))
	{
		Location = MeshComp->GetSocketLocation(AttachName);
	}
	else
	{
		Location = MeshComp->GetComponentLocation();
	}

	// 3D 사운드 재생
	UGameplayStatics::PlaySoundAtLocation(
		MeshComp->GetWorld(),
		Sound,
		Location,
		VolumeMultiplier,
		PitchMultiplier
	);
}

FString UAnimNotify_PlaySound3D::GetNotifyName_Implementation() const
{
	if (Sound)
	{
		return FString::Printf(TEXT("Sound 3D : %s"), *Sound->GetName());
	}

	return TEXT("Sound 3D : None");
}
