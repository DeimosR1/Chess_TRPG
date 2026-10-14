// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/CreateServerButton.h"

#include "Components/Button.h"

void UCreateServerButton::NativeConstruct()
{
	Super::NativeConstruct();
	ConfirmButton->OnClicked.AddDynamic(this, &UCreateServerButton::ConfirmButtonClicked);
	MultiplayerJoiningHandler = GetGameInstance()->GetSubsystem<UMultiplayerGameLogicHandler>();
}

void UCreateServerButton::ConfirmButtonClicked()
{
	MultiplayerJoiningHandler->CreateServerAsListener(ServerName.ToString(), LevelPath);
}
