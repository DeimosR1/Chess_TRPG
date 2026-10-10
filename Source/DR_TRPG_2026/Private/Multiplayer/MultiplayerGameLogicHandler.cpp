// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/MultiplayerGameLogicHandler.h"

#include "Kismet/GameplayStatics.h"

UMultiplayerGameLogicHandler::UMultiplayerGameLogicHandler()
{
}


void UMultiplayerGameLogicHandler::JoinServerAsClient(APlayerController* Controller)
{
	Controller->ClientTravel(IPAddress.ToString(),TRAVEL_Absolute);
}

void UMultiplayerGameLogicHandler::CreateServerAsListener(UWorld* World)
{
	UGameplayStatics::OpenLevel(World, LevelName, true, "listen");
}
