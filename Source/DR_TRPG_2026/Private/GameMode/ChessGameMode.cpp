// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ChessGameMode.h"

#include "ChessGameState.h"
#include "GamePieces/PChessPiece.h"
#include "GridMovement/ChessBoardManager.h"
#include "Kismet/GameplayStatics.h"
#include "Multiplayer/ChessPlayerState.h"

void AChessGameMode::BeginPlay()
{
	Super::BeginPlay();
	ChessGameState = GetGameState<AChessGameState>();
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
	
	if (PlayerUnits.Num() <= 0 || PlayerTwoUnits.Num() <= 0)
	{
		//Temporary
		UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(this, 0), EQuitPreference::Quit,false);
		return;
	}
	
	if (bIsPlayerTurn == true)
	{
		for (APChessPiece* ChessPiece : PlayerUnits)
		{
			if (!IsValid(ChessPiece)) { continue; }
			
			if (!ChessPiece->bHasMoved)
			{
				bAllUnitsMoved = false;
				break;
			}
		}
	}
	else
	{
		for (APChessPiece* ChessPiece : PlayerTwoUnits)
		{
			if (!IsValid(ChessPiece)) { continue; }
			
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
	SyncGameState();
}

void AChessGameMode::AddUnits()
{
	PlayerUnits.Empty();
	PlayerTwoUnits.Empty();
	//We find all the registered Units in the Manager and add them here to control the game logic.
	TArray<APChessPiece*> ChessPiecesOnBoard = ChessBoardManager->GetAllPiecesOnBoard();
	for (APChessPiece* ChessPiece : ChessPiecesOnBoard)
	{
		if (!IsValid(ChessPiece)){continue;}
		
		if (ChessPiece->bIsPlayerTeam == true)
		{
			PlayerUnits.Add(ChessPiece);
			PlayerPieceCount = PlayerUnits.Num();
		}
		else
		{
			PlayerTwoUnits.Add(ChessPiece);
			PlayerTwoPieceCount = PlayerTwoUnits.Num();
		}
	}
	
	SyncGameState();
}

void AChessGameMode::EndCurrentTurn()
{
	if (bIsPlayerTurn == true)
	{
		for (APChessPiece* ChessPiece : PlayerUnits)
		{
			if (!IsValid(ChessPiece)) { continue; }
			ChessPiece->ChangeHasMoved(true);
		}
	}
	else
	{
		for (APChessPiece* ChessPiece : PlayerTwoUnits)
		{
			if (!IsValid(ChessPiece)) { continue; }
			ChessPiece->ChangeHasMoved(true);
		}
	}
	CheckIfTurnChange();
	SyncGameState();
}

void AChessGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	// Game Only supports two players, if a third tries to join tell them it's full
	if (ErrorMessage.IsEmpty() && GameState && GameState->PlayerArray.Num() >= 2)
	{
		ErrorMessage = TEXT("Match is full.");
	}
}

void AChessGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	AChessPlayerState* NewPlayerState = NewPlayer ? NewPlayer->GetPlayerState<AChessPlayerState>() : nullptr;
	
	if (!NewPlayerState) { return; }

	// Find out which sides are already taken by other connected players
	bool bLightTaken = false;
	bool bDarkTaken = false;

	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		const AChessPlayerState* OtherPlayer = Cast<AChessPlayerState>(PlayerState);
		
		if (!OtherPlayer || OtherPlayer == NewPlayerState || OtherPlayer->IsInactive() || !OtherPlayer->HasSideAssigned()) { continue; }

		if (OtherPlayer->IsPlayerLight()) { bLightTaken = true; }
		else { bDarkTaken = true; }
	}

	// The First player in takes the light pieces, the second gets the dark pieces
	if (!bLightTaken)
	{
		NewPlayerState->AssignSide(true);
	}
	else if (!bDarkTaken)
	{
		NewPlayerState->AssignSide(false);
	}
}

void AChessGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	//Temporary
	UE_LOG(LogTemp, Warning, TEXT("Logged Out"));
}

void AChessGameMode::AllowUnitsToMoveAgain()
{
	if (bIsPlayerTurn == true)
	{
		for (APChessPiece* ChessPiece : PlayerUnits)
		{
			if (!IsValid(ChessPiece)) { continue; }
			ChessPiece->ChangeHasMoved(false);
		}
	}
	else
	{
		for (APChessPiece* ChessPiece : PlayerTwoUnits)
		{
			if (!IsValid(ChessPiece)) { continue; }
			ChessPiece->ChangeHasMoved(false);
		}
	}
	
	for (APChessPiece* ChessPiece : PlayerUnits)
	{
		ChessPiece->ChangeActiveColor(false);
	}
	for (APChessPiece* ChessPiece : PlayerTwoUnits)
	{
		ChessPiece->ChangeActiveColor(false);
	}
	
	SyncGameState();
}

void AChessGameMode::SyncGameState() const
{	
	if (IsValid(ChessGameState))
	{
		ChessGameState->OnTurnChanged(bIsPlayerTurn);
		ChessGameState->TurnCount = TurnCount;
		ChessGameState->PlayerPieceCount = PlayerPieceCount;
		ChessGameState->PlayerTwoPieceCount = PlayerTwoPieceCount;
	}	
}
