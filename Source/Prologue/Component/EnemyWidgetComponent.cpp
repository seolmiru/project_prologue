// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWidgetComponent.h"

#include "Prologue/UI/PrologueUserWidget.h"

void UEnemyWidgetComponent::InitWidget()
{
	Super::InitWidget();

	UPrologueUserWidget* PrologueUserWidget = Cast<UPrologueUserWidget>(GetWidget());

	if (PrologueUserWidget)
	{
		PrologueUserWidget->SetAbilitySystemComponent(GetOwner());
	}
}
