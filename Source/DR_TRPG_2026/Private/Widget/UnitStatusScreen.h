// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePieces/PChessPiece.h"
#include "UnitStatusScreen.generated.h"

class ATPlayerController;
/**
 * 
 */
UCLASS()
class UUnitStatusScreen : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	UFUNCTION()
	void SetValueOfHealthBar(int32 Value, int32 MaxValue);
	UFUNCTION(BlueprintCallable)
	void SetSelectedUnit(APChessPiece* Piece, AChessPlayerState* RequestingPlayerState);

	UFUNCTION(BlueprintCallable)
	void ClearSelectedUnit();

private:
	UPROPERTY()
	TObjectPtr<APChessPiece> SelectedPiece;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* UnitName;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* UnitAttack;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* UnitDefense;
	UPROPERTY(meta = (BindWidget))
	class UValueGauge* HealthBar;
};
