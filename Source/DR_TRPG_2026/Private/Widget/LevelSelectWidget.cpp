// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/LevelSelectWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void ULevelSelectWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	LevelName->SetText(FText::FromString(HeldLevel.GetAssetName()));
	LevelDescription->SetText(Description);
	
	if (Icon) { LevelIcon->SetBrushFromTexture(Icon); }
}

void ULevelSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	LevelName->SetText(FText::FromString(HeldLevel.GetAssetName()));
	LevelDescription->SetText(Description);
	
	if (Icon) { LevelIcon->SetBrushFromTexture(Icon); }
	
	SelectButton->OnClicked.AddDynamic(this, &ULevelSelectWidget::OnHandleClicked);
}

void ULevelSelectWidget::OnHandleClicked()
{
	OnLevelSelected.Broadcast(this);
}
