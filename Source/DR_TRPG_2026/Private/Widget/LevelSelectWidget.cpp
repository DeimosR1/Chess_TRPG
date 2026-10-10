// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/LevelSelectWidget.h"

#include "Components/TextBlock.h"

void ULevelSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (LevelName) {LevelName->SetText(FText::FromString(HeldLevel->GetName()));}
}
