// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/CreateServerMainMenuButton.h"

#include "Components/Button.h"

void UCreateServerMainMenuButton::NativeConstruct()
{
	Super::NativeConstruct();
	CreateServerMainMenuButton->OnClicked.AddDynamic(this, &UCreateServerMainMenuButton::OnCreateServerMainMenuButtonClicked);
}

void UCreateServerMainMenuButton::OnCreateServerMainMenuButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Create Clicked"))
	OnClickedCreate.Broadcast();
}
