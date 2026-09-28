// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/UnitStatusScreen.h"

#include "ValueGauge.h"
#include "Components/TextBlock.h"

void UUnitStatusScreen::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::Collapsed);
}

void UUnitStatusScreen::SetValueOfHealthBar(int32 Value, int32 MaxValue)
{
	HealthBar->SetValue(Value, MaxValue);
}

void UUnitStatusScreen::SetSelectedUnit(APChessPiece* Piece)
{
	if (!IsValid(Piece))
	{
		ClearSelectedUnit();
		return;
	}

	SelectedPiece = Piece;

	if (UnitName)    { UnitName->SetText(FText::FromName(Piece->GetCurrentUnitDisplayName())); }
	if (UnitAttack)  { UnitAttack->SetText(FText::AsNumber(Piece->GetDamage())); }
	if (UnitDefense) { UnitDefense->SetText(FText::AsNumber(Piece->GetDefense())); }

	SetValueOfHealthBar(Piece->GetCurrentHealth(), Piece->GetMaxHealth());

	SetVisibility(ESlateVisibility::Visible);
}

void UUnitStatusScreen::ClearSelectedUnit()
{
	SelectedPiece = nullptr;
	SetVisibility(ESlateVisibility::Collapsed);
}
