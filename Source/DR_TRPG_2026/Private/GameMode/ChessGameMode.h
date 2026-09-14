// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ChessGameMode.generated.h"

class AChessBoardManager;
class APChessPiece;
/**
 * 
 */
UCLASS()
class AChessGameMode : public AGameMode
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(VisibleAnywhere)
	TArray<APChessPiece*> PlayerUnits;
	UPROPERTY(VisibleAnywhere)
	TArray<APChessPiece*> EnemyUnits;
	UPROPERTY(VisibleAnywhere)
	float TurnCount = 0;
	UPROPERTY(VisibleAnywhere)
	bool bIsPlayerTurn = true;
	
	UFUNCTION(BlueprintCallable)
	void CheckIfTurnChange();
	
	UFUNCTION(BlueprintCallable)
	void AddUnits();
	
	UFUNCTION(BlueprintCallable)
	void EndCurrentTurn();
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AChessBoardManager> ChessBoardManager;
	
	UPROPERTY(VisibleAnywhere)
	int32 PlayerPieceCount;
	UPROPERTY(VisibleAnywhere)
	int32 EnemyPieceCount;
	
	UFUNCTION(BlueprintCallable)
	void AllowUnitsToMoveAgain();
};
