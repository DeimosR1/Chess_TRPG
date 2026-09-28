// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ChessGameState.h"

#include "Net/UnrealNetwork.h"

void AChessGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AChessGameState, bIsPlayerTurn);
	DOREPLIFETIME(AChessGameState, TurnCount);
	DOREPLIFETIME(AChessGameState, PlayerPieceCount);
	DOREPLIFETIME(AChessGameState, PlayerTwoPieceCount);
	
}

void AChessGameState::OnTurnChanged(bool bNewIsPlayerTurn)
{
	if (bIsPlayerTurn == bNewIsPlayerTurn)
	{
		return;
	}

	bIsPlayerTurn = bNewIsPlayerTurn;
	OnTurnChangedDelegate.Broadcast(bIsPlayerTurn);
}

void AChessGameState::OnRep_IsPlayerTurn() const
{
	OnTurnChangedDelegate.Broadcast(bIsPlayerTurn);
}
