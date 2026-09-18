// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MultiplayerRelayTower.generated.h"

class APChessPiece;
class AChessBoardManager;


//Look into PlayerState or an ActorComponent for this
//Player State is better cause it persists 
//Research RPC

UCLASS()
class AMultiplayerRelayTower : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMultiplayerRelayTower();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//This all the variables we need for multiplayer to function
	
	//The Chessboard manager that exists only in the server
	UPROPERTY()
	TObjectPtr<AChessBoardManager> ChessBoardManagerServer;
	
	//To check if the player is currently being the light pieces, helps with changing certain variables
	UPROPERTY()
	bool bIsPlayerLight;
	
	//Function to send over the selected Piece of the player to the server.
	UFUNCTION()
	void GetAndRelayClientSelectedPiece(APChessPiece* SelectedPiece);
	
};
