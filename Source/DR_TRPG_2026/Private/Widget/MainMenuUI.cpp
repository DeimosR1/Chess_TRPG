// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainMenuUI.h"

#include "CreateServerMainMenuButton.h"
#include "CreateServerUI.h"
#include "JoinServerUI.h"

void UMainMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
	CreateServerMainMenuButton->OnClickedCreate.AddDynamic(this, &UMainMenuUI::OnCreateServerUIActivateButtonClicked);
}

void UMainMenuUI::OnJoinServerUIActivateButtonClicked()
{
	JoinServerUI->SetVisibility(ESlateVisibility::Hidden);
}

void UMainMenuUI::OnCreateServerUIActivateButtonClicked()
{
	CreateServerUI->SetVisibility(ESlateVisibility::Hidden);
}
