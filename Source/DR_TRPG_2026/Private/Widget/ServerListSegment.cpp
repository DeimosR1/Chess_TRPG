// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/ServerListSegment.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Multiplayer/MultiplayerGameLogicHandler.h"

void UServerListSegment::NativeConstruct()
{
	Super::NativeConstruct();
	UGameInstance* GameInstance = GetGameInstance();
	
	if (GameInstance)
	{
		MultiplayerJoiningHandler = GameInstance->GetSubsystem<UMultiplayerGameLogicHandler>();
	}
	
	ConfirmButton->OnClicked.AddDynamic(this, &UServerListSegment::OnConfirmClicked);
	SetDisplayName();

}

void UServerListSegment::SetDisplayName()
{
	PureTextServerName = FString::Printf(TEXT("Server %d"), SessionIndex);
	ServerName->SetText(FText::FromString(PureTextServerName));
}

void UServerListSegment::OnConfirmClicked()
{
	if (MultiplayerJoiningHandler)
	{
		MultiplayerJoiningHandler->JoinSessionsByIndex(SessionIndex);
	}
}
