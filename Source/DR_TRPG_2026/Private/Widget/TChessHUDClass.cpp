// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/TChessHUDClass.h"

#include "ChessGeneralHUD.h"
#include "Blueprint/UserWidget.h"

void ATChessHUDClass::BeginPlay()
{
	Super::BeginPlay();
	if (ChessHUDWidgetClass)
	{
		ChessHUDWidget = CreateWidget<UChessGeneralHUD>(GetOwningPlayerController(), ChessHUDWidgetClass);
		if (ChessHUDWidget)
		{
			ChessHUDWidget->AddToViewport();
		}
	}
}
