// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/ChessGeneralHUD.h"

#include "BackToMainMenuWidget.h"
#include "TurnStatusChangeWidget.h"
#include "UnitStatusScreen.h"
#include "GameMode/ChessGameState.h"
#include "GridMovement/ChessBoardManager.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerCharacter.h"

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
	
	APawn* LocalPawn = nullptr;
	if (ULocalPlayer* LocalPlayer = GetOwningLocalPlayer())
	{
		if (APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld()))
		{
			LocalPawn = PlayerController->GetPawn();
			if (LocalPawn)
			{
				APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(LocalPawn);
				if (PlayerCharacter)
				{
					PlayerCharacter->OnQuitGamePressed.AddDynamic(this, &UChessGeneralHUD::ShowBackToMainMenuMenu);
				}
			}
		}
	}
}

void UChessGeneralHUD::HandleSelectedPieceChanged(APChessPiece* NewSelectedPiece, AChessPlayerState* RequestingPlayerState)
{
	if (!UnitStatusScreen) return;

	if (IsValid(NewSelectedPiece))
	{
		UnitStatusScreen->SetSelectedUnit(NewSelectedPiece, RequestingPlayerState);
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

void UChessGeneralHUD::ShowBackToMainMenuMenu()
{
	BackToMainMenuWidget->SetVisibility(ESlateVisibility::Visible);
}
