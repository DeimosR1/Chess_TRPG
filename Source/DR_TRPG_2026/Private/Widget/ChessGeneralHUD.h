// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChessGeneralHUD.generated.h"

/**
 * 
 */
UCLASS()
class UChessGeneralHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta=(BindWidget))
	class UUnitStatusScreen* UnitStatusScreen;

	UPROPERTY(meta=(BindWidget))
	class UTurnStatusChangeWidget* TurnStatusChangeWidget;
	
	UPROPERTY(meta=(BindWidget))
	class UBackToMainMenuWidget* BackToMainMenuWidget;

private:
	UPROPERTY()
	TObjectPtr<class AChessBoardManager> ChessBoardManager;
	UPROPERTY()
	TObjectPtr<class AChessGameState> ChessGameState;
	
	//Keep just in case
	UPROPERTY()
	bool bBackIsActive = false;

	UFUNCTION()
	void HandleSelectedPieceChanged(class APChessPiece* NewSelectedPiece, class AChessPlayerState* RequestingPlayerState );
	

	UFUNCTION()
	void HandleTurnChanged(bool bNewIsPlayerTurn);
	
	UFUNCTION()
	void ShowBackToMainMenuMenu();
};
