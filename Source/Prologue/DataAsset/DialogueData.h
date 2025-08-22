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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FString SpeakerName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	ESpeakerType SpeakerType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FText DialogueText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	EDialogueType DialogueType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FName NextDialogueID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	float Duration;
};
