// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TPlayerController.h"

void ATPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	this->bShowMouseCursor = true;
	this->bEnableClickEvents = true;
	this->bEnableMouseOverEvents = true;
	
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	
}
