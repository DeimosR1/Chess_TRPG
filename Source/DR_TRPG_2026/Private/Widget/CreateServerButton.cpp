// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/CreateServerButton.h"

void UCreateServerButton::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCreateServerButton::ConfirmButtonClicked()
{
	MultiplayerJoiningHandler->SetLevelName(FName(SelectedLevel->GetName()));
	MultiplayerJoiningHandler->CreateServerAsListener(GetWorld());
}
