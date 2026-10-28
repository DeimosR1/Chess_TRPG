// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/BackToMainMenuWidget.h"

#include "Components/Button.h"
#include "Multiplayer/MultiplayerGameLogicHandler.h"

class UMultiplayerGameLogicHandler;

void UBackToMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BackToMainMenuButton)
	{
		BackToMainMenuButton->OnClicked.AddDynamic(this, &UBackToMainMenuWidget::OnBackToMainClicked);
	}
	if (CancelOperation)
	{
		CancelOperation->OnClicked.AddDynamic(this, &UBackToMainMenuWidget::OnCancelOperationClicked);
	}
	
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UBackToMainMenuWidget::OnBackToMainClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Back to Main Menu Clicked"));
	this->SetVisibility(ESlateVisibility::Hidden);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UMultiplayerGameLogicHandler* LogicHandler = GameInstance->GetSubsystem<UMultiplayerGameLogicHandler>())
		{
			LogicHandler->LeaveGameAndReturnToMenu();
		}
	}
}

void UBackToMainMenuWidget::OnCancelOperationClicked()
{
	this->SetVisibility(ESlateVisibility::Hidden);
}
