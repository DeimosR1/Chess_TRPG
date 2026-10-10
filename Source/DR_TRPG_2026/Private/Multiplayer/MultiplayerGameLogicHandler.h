// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiplayerGameLogicHandler.generated.h"

// This is a Script that will hold all the Joining and Creating Level ata neccesarry for the system to open and close the game levels

/**
 * 
 */
UCLASS()
class UMultiplayerGameLogicHandler : public UObject
{
	GENERATED_BODY()
public:
	UMultiplayerGameLogicHandler();
	
	//Holds the Level name
	UPROPERTY(EditDefaultsOnly, Category = "Multiplayer")
	FName LevelName;
	UPROPERTY(EditDefaultsOnly, Category = "Multiplayer")
	FName IPAddress;
	
	//Called when pressing Join, it gets the Controller to then send it to the IP Adress
	UFUNCTION()
	void JoinServerAsClient(APlayerController* Controller);
	
	//Called when pressing Confirm when creating a server
	UFUNCTION()
	void CreateServerAsListener(UWorld* World);
	
	UFUNCTION()
	void SetLevelName(FName NewLevelName){LevelName = NewLevelName;};
	UFUNCTION()
	void SetIPAddress(FName NewIPAddress){IPAddress = NewIPAddress;};
};
