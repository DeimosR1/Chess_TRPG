// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/JoinServerUI.h"

#include "Multiplayer/MultiplayerGameLogicHandler.h"

void UJoinServerUI::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UJoinServerUI::NativeConstruct()
{
	Super::NativeConstruct();
}

void UJoinServerUI::SearchForServers()
{
	MultiplayerJoiningHandler->StartGameSessionSearch();
}
