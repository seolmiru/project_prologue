// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeGhostChat.h"

#include "Components/TextBlock.h"

void UTimeGhostChat::SetChatMessage(const FText& Message)
{
	ChatMessage = Message;

	if (MessageText)
	{
		MessageText->SetText(Message);
	}
}
