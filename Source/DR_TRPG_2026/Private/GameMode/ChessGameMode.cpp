// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ChessGameMode.h"

#include "GamePieces/PChessPiece.h"
#include "GridMovement/ChessBoardManager.h"
#include "Kismet/GameplayStatics.h"

void AChessGameMode::BeginPlay()
{
	Super::BeginPlay();
	ChessBoardManager = Cast<AChessBoardManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AChessBoardManager::StaticClass()));
	if (IsValid(ChessBoardManager))
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &AChessGameMode::AddUnits);
	}
}

void AChessGameMode::CheckIfTurnChange()
{
	UE_LOG(LogTemp, Warning, TEXT("Checking if Turn Changed"));
	bool bAllUnitsMoved = true;
	
	if (bIsPlayerTurn == true)
	{
		for (APChessPiece* ChessPiece : PlayerUnits)
		{
			if (!ChessPiece->bHasMoved)
			{
				bAllUnitsMoved = false;
				break;
			}
		}
	}
	else
	{
		for (APChessPiece* ChessPiece : EnemyUnits)
		{
			if (!ChessPiece->bHasMoved)
			{
				bAllUnitsMoved = false;
				break;
			}
		}
	}
	
	if (bAllUnitsMoved == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("Changing Turn"));
		bIsPlayerTurn = !bIsPlayerTurn;
		TurnCount++;
		AllowUnitsToMoveAgain();
	}
	AddUnits();
}

void AChessGameMode::AddUnits()
{
	PlayerUnits.Empty();
	EnemyUnits.Empty();
	//We find all the registered Units in the Manager and add them here to control the game logic.
	TArray<APChessPiece*> ChessPiecesOnBoard = ChessBoardManager->GetAllPiecesOnBoard();
	for (APChessPiece* ChessPiece : ChessPiecesOnBoard)
	{
		if (ChessPiece->bIsPlayerTeam == true)
		{
			PlayerUnits.Add(ChessPiece);
			PlayerPieceCount = PlayerUnits.Num();
		}
		else
		{
			EnemyUnits.Add(ChessPiece);
			EnemyPieceCount = EnemyUnits.Num();
		}
	}
}

void AChessGameMode::EndCurrentTurn()
{
	if (bIsPlayerTurn == true)
	{
		for (APChessPiece* ChessPiece : PlayerUnits)
		{
			ChessPiece->ChangeHasMoved(true);
		}
	}
	else
	{
		for (APChessPiece* ChessPiece : EnemyUnits)
		{
			ChessPiece->ChangeHasMoved(true);
		}
	}
	CheckIfTurnChange();
}

void AChessGameMode::AllowUnitsToMoveAgain()
{
	if (bIsPlayerTurn == true)
	{
		for (APChessPiece* ChessPiece : PlayerUnits)
		{
			ChessPiece->ChangeHasMoved(false);
		}
	}
	else
	{
		for (APChessPiece* ChessPiece : EnemyUnits)
		{
			ChessPiece->ChangeHasMoved(false);
		}
	}
}
