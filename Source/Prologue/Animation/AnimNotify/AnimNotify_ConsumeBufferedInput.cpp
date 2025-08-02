// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_ConsumeBufferedInput.h"

#include "Prologue/Component/InputBufferComponent.h"

void UAnimNotify_ConsumeBufferedInput::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (UInputBufferComponent* InputBuffer = MeshComp->GetOwner()->FindComponentByClass<UInputBufferComponent>())
		{
			if (bConsumeSpecificTag && SpecificInputTag.IsValid())
			{
				if (InputBuffer->HasBufferedInput(SpecificInputTag))
				{
					InputBuffer->ConsumeBufferedInput();
				}
			}
			else
			{
				InputBuffer->ConsumeBufferedInput();
			}
		}
	}
}
