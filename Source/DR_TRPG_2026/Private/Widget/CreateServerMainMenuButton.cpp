// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/CreateServerMainMenuButton.h"

void UCreateServerMainMenuButton::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCreateServerMainMenuButton::OnCreateServerMainMenuButtonClicked()
{
	OnClickedCreate.Broadcast();
}
