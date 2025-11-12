#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DialogueData.generated.h"

UENUM(BlueprintType)
enum class ESpeakerType : uint8
{
	NPC = 0,
	System = 1
};

UENUM(BlueprintType)
enum class EDialogueType : uint8
{
	Normal = 0,
	End = 1
};

USTRUCT(BlueprintType)
struct FDialogueData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	FString SpeakerName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	ESpeakerType SpeakerType = ESpeakerType::NPC;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	FText DialogueText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	EDialogueType DialogueType = EDialogueType::Normal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	FName NextDialogueID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialouge|Voice")
	TSoftObjectPtr<USoundBase> SpeakerVoice;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialouge|Image")
	TSoftObjectPtr<UTexture2D> CutSceneImage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	float Duration = 0.f;

	
};
