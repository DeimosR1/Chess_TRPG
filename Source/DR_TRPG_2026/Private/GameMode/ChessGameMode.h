// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ChessGameMode.generated.h"

class AChessGameState;
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
	TArray<APChessPiece*> PlayerTwoUnits;
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
	
	// For player Joining
	virtual void PreLogin(const FString& Options, const FString& Address,
	const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AChessBoardManager> ChessBoardManager;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AChessGameState> ChessGameState;
	
	UPROPERTY(VisibleAnywhere)
	int32 PlayerPieceCount;
	UPROPERTY(VisibleAnywhere)
	int32 PlayerTwoPieceCount;
	
	UFUNCTION(BlueprintCallable)
	void AllowUnitsToMoveAgain();
	
	//To replicate the values in the GameState
	UFUNCTION()
	void SyncGameState() const;
};
