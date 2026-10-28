// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainMenuUI.h"

#include "CreateServerMainMenuButton.h"
#include "CreateServerUI.h"
#include "JoinServerUI.h"
#include "Components/Button.h"
#include "Player/PlayerCharacter.h"

void UMainMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
	CreateServerMainMenuButton->OnClickedCreate.AddDynamic(this, &UMainMenuUI::OnCreateServerUIActivateButtonClicked);
	JoinServerUIActivateButton->OnClicked.AddDynamic(this, &UMainMenuUI::OnJoinServerUIActivateButtonClicked);
	CreateServerUI->SetVisibility(ESlateVisibility::Hidden);
	JoinServerUI->SetVisibility(ESlateVisibility::Hidden);
}

void UMainMenuUI::OnJoinServerUIActivateButtonClicked()
{
	JoinServerUI->SetVisibility(ESlateVisibility::Visible);
	CreateServerUI->SetVisibility(ESlateVisibility::Hidden);
}

void UMainMenuUI::OnCreateServerUIActivateButtonClicked()
{
	CreateServerUI->SetVisibility(ESlateVisibility::Visible);
	JoinServerUI->SetVisibility(ESlateVisibility::Hidden);
}
