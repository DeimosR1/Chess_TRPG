// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/ChessGeneralHUD.h"

#include "TurnStatusChangeWidget.h"
#include "UnitStatusScreen.h"
#include "GameMode/ChessGameState.h"
#include "GridMovement/ChessBoardManager.h"
#include "Kismet/GameplayStatics.h"

void UChessGeneralHUD::NativeConstruct()
{
	Super::NativeConstruct();

	if (UnitStatusScreen)
	{
		UnitStatusScreen->ClearSelectedUnit();
	}

	ChessBoardManager = Cast<AChessBoardManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AChessBoardManager::StaticClass()));
	if (ChessBoardManager)
	{
		ChessBoardManager->OnSelectedPieceChanged.AddDynamic(this, &UChessGeneralHUD::HandleSelectedPieceChanged);
	}

	ChessGameState = GetWorld()->GetGameState<AChessGameState>();
	if (ChessGameState)
	{
		ChessGameState->OnTurnChangedDelegate.AddDynamic(this, &UChessGeneralHUD::HandleTurnChanged);
	}
}

void UChessGeneralHUD::HandleSelectedPieceChanged(APChessPiece* NewSelectedPiece)
{
	if (!UnitStatusScreen) return;

	if (IsValid(NewSelectedPiece))
	{
		UnitStatusScreen->SetSelectedUnit(NewSelectedPiece);
	}
	else
	{
		UnitStatusScreen->ClearSelectedUnit();
	}
}

void UChessGeneralHUD::HandleTurnChanged(bool bNewIsPlayerTurn)
{
	if (TurnStatusChangeWidget)
	{
		TurnStatusChangeWidget->PlayTurnChangeNotice(bNewIsPlayerTurn);
	}
}
