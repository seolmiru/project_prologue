// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "TutorialGuide.generated.h"


// 월드에 조작키를 알려주는 튜토리얼 가이드 형태의 UI를 띄어줌
// 버그 때문에 미사용
UCLASS()
class PROLOGUE_API ATutorialGuide : public AActor
{
	GENERATED_BODY()

public:
	ATutorialGuide();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* GuideWidgetComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> BP_GuideWidget;

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowGuide();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideGuide();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void DestroyGuide();
};
