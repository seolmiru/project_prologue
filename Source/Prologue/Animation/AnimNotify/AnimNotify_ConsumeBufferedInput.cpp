// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_ConsumeBufferedInput.h"

#include "Prologue/Component/InputBufferComponent.h"

void UAnimNotify_ConsumeBufferedInput::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (UInputBufferComponent* InputBuffer = MeshComp->GetOwner()->FindComponentByClass<UInputBufferComponent>())
		{
			if (bConsumeSpecificTag && !SpecificInputTag.IsEmpty())
			{
				if (bConsumeFirst)
				{
					for (const FGameplayTag& Tag : SpecificInputTag)
					{
						if (InputBuffer->HasBufferedInput(Tag))
						{
							InputBuffer->ConsumeBufferedInput();
							break;
						}
					}
				}
				else
				{
					bool bConsumeAny = false;
					for (const FGameplayTag& Tag : SpecificInputTag)
					{
						while (InputBuffer->HasBufferedInput(Tag))
						{
							InputBuffer->ConsumeBufferedInput();
							bConsumeAny = true;
						}

						if (bConsumeAny)
							break;
					}
				}
			}
			else
			{
				InputBuffer->ConsumeBufferedInput();
			}
		}
	}
}
