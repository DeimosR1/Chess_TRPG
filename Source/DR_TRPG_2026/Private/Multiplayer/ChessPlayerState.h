// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GridMovement/ChessBoardManager.h"
#include "ChessPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class AChessPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
private:
	//This all the variables we need for multiplayer to function
	
	//The Chessboard manager that exists only in the server
	UPROPERTY()
	TObjectPtr<AChessBoardManager> ChessBoardManagerServer;
	
	UPROPERTY()
	TObjectPtr<class AChessGameMode> GameModeClass;
	
	//To check if the player is currently being the light pieces, helps with changing certain variables
	UPROPERTY(ReplicatedUsing=OnRep_Side)
	bool bIsPlayerLight = true;
	UPROPERTY(Replicated)
	bool bSideAssigned = false;
	
public:
	
	bool IsPlayerLight() const { return bIsPlayerLight; }
	bool HasSideAssigned() const { return bSideAssigned; }

	// Server only. Called by the GameMode when the player joins.
	UFUNCTION()
	void AssignSide(bool bPlayer01);
	
	// Called by the Client so the server selects the piece after checking for ownership
	UFUNCTION(Server, Reliable)
	void ServerSelectPiece(APChessPiece* SelectedPiece);
 
	//Called by the Client to tell the server to move the piece to the tile
	UFUNCTION(Server, Reliable)
	void ServerTryMoveSelectedPieceTo(FIntPoint TargetTile);
 
	// Called by the owning client to ask the server to end this side's turn.
	UFUNCTION(Server, Reliable)
	void ServerEndCurrentTurn();
	
	UFUNCTION(Server, Reliable)
	void ServerDeselectPiece();
 
	UFUNCTION()
	void EndCurrentTurn();
	
	UFUNCTION()
	void OnRep_Side();
 
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
