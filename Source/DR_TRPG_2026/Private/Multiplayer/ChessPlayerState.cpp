// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/ChessPlayerState.h"

#include "GameFramework/GameStateBase.h"
#include "GameMode/ChessGameMode.h"
#include "GamePieces/PChessPiece.h"
#include "GridMovement/ChessBoardManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/PlayerCharacter.h"

void AChessPlayerState::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		ChessBoardManagerServer = Cast<AChessBoardManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AChessBoardManager::StaticClass()));
		GameModeClass = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	}
	
}

void AChessPlayerState::AssignSide(bool bPlayer01)
{
	if (!HasAuthority()) { return; }
	
	bIsPlayerLight = bPlayer01;
	bSideAssigned = true;
	OnRep_Side();
}

void AChessPlayerState::ServerDeselectPiece_Implementation()
{
	if (IsValid(ChessBoardManagerServer))
	{
		ChessBoardManagerServer->DeselectPiece(this);
	}
}

void AChessPlayerState::EndCurrentTurn()
{
	//Reroute to end the current turn in the server if called from client
	ServerEndCurrentTurn();
}

void AChessPlayerState::OnRep_Side()
{
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn()))
	{
		PlayerCharacter->SetupPlayer();
	}
}

void AChessPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AChessPlayerState, bIsPlayerLight);
	DOREPLIFETIME(AChessPlayerState, bSideAssigned);
}

void AChessPlayerState::ServerEndCurrentTurn_Implementation()
{
	if (IsValid(GameModeClass))
	{
		if (!bSideAssigned) { return; }

		// Only the side whose turn it is may end it
		if (bIsPlayerLight != GameModeClass->bIsPlayerTurn) { return; }

		GameModeClass->EndCurrentTurn();
	}
}

void AChessPlayerState::ServerTryMoveSelectedPieceTo_Implementation(FIntPoint TargetTile)
{
	if (IsValid(ChessBoardManagerServer))
	{
		// The Manager checks if this is the player that selected the piece, so we sent it.
		ChessBoardManagerServer->TryMoveSelectedPieceTo(TargetTile, this);
	}
}

void AChessPlayerState::ServerSelectPiece_Implementation(APChessPiece* SelectedPiece)
{
	if (!IsValid(ChessBoardManagerServer) || !IsValid(SelectedPiece))
	{
		return;
	}
	
	if (!bSideAssigned) {return;}
	
	// Never let a client select a piece that isn't theirs, regardless of whose turn it is.
	if (SelectedPiece->bIsPlayerTeam != bIsPlayerLight)
	{
		return;
	}
 
	ChessBoardManagerServer->SelectPiece(SelectedPiece, this);
}

