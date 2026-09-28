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

private:
	UPROPERTY()
	TObjectPtr<class AChessBoardManager> ChessBoardManager;
	UPROPERTY()
	TObjectPtr<class AChessGameState> ChessGameState;

	UFUNCTION()
	void HandleSelectedPieceChanged(class APChessPiece* NewSelectedPiece);

	UFUNCTION()
	void HandleTurnChanged(bool bNewIsPlayerTurn);
};
