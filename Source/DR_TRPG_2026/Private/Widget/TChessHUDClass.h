// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TChessHUDClass.generated.h"

/**
 * 
 */
UCLASS()
class ATChessHUDClass : public AHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class UChessGeneralHUD> ChessHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<class UChessGeneralHUD> ChessHUDWidget;
	
};
