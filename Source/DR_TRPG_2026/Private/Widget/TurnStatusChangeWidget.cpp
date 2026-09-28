// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/TurnStatusChangeWidget.h"

#include "Components/TextBlock.h"

void UTurnStatusChangeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::Collapsed);
}

void UTurnStatusChangeWidget::PlayTurnChangeNotice(bool bIsPlayerTurn)
{
	if (PlayerTurn) { PlayerTurn->SetVisibility(bIsPlayerTurn ? ESlateVisibility::Visible : ESlateVisibility::Collapsed); }
	if (EnemyTurn)  { EnemyTurn->SetVisibility(bIsPlayerTurn ? ESlateVisibility::Collapsed : ESlateVisibility::Visible); }

	SetVisibility(ESlateVisibility::Visible);

	GetWorld()->GetTimerManager().ClearTimer(HideTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(HideTimerHandle, this, &UTurnStatusChangeWidget::HideNotice, DisplayDuration, false);
}


void UTurnStatusChangeWidget::HideNotice()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
