// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ChessGameState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnChangedNative, bool, bNewIsPlayerTurn);

UCLASS()
class AChessGameState : public AGameState
{
	GENERATED_BODY()
public:
	// True if it's currently the light/player side's turn.
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing=OnRep_IsPlayerTurn, Category="Turn")
	bool bIsPlayerTurn = true;
 
	UPROPERTY(EditDefaultsOnly, Replicated, Category="Turn")
	int32 TurnCount = 0;
 
	UPROPERTY(EditDefaultsOnly, Replicated, Category="Turn")
	int32 PlayerPieceCount = 0;
 
	UPROPERTY(EditDefaultsOnly, Replicated, Category="Turn")
	int32 PlayerTwoPieceCount = 0;
 
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
 
	// UI
	
	UFUNCTION(Category="Turn")
	void OnTurnChanged(bool bNewIsPlayerTurn);
	
	UPROPERTY(BlueprintAssignable, Category="Turn")
	FOnTurnChangedNative OnTurnChangedDelegate;
 
protected:
	//To replicate on the client and check if it's player turn
	UFUNCTION()
	void OnRep_IsPlayerTurn() const;
};
