// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/CreateServerUI.h"

#include "CreateServerButton.h"
#include "LevelSelectWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"

void UCreateServerUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (BackButton) {BackButton->OnClicked.AddDynamic(this, &UCreateServerUI::OnBackPressed); }
	
	for (int i = 0; i < LevelScrollBox->GetChildrenCount(); i++)
	{
		ULevelSelectWidget* Widget = Cast<ULevelSelectWidget>(LevelScrollBox->GetChildAt(i));
		Widget->OnLevelSelected.AddDynamic(this, &UCreateServerUI::LevelSelectedHandler);
	}
	
	if (ServerNameTextBox){ServerNameTextBox->OnTextChanged.AddDynamic(this, &UCreateServerUI::OnTextChanged);}
	CreateServerButton->SetIsEnabled(false);
}

void UCreateServerUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (!LevelScrollBox) return;

	// Clear existing children to avoid duplicates on recompile
	LevelScrollBox->ClearChildren();

	for (TSubclassOf<ULevelSelectWidget>& LevelSelectorClass : LevelSelectorWidgets)
	{
		if (!LevelSelectorClass) {continue;}
		
		ULevelSelectWidget* LevelSelector = CreateWidget<ULevelSelectWidget>(this, LevelSelectorClass);
		LevelScrollBox->AddChild(LevelSelector);
	}
}

void UCreateServerUI::OnBackPressed()
{
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UCreateServerUI::LevelSelectedHandler(ULevelSelectWidget* LevelSelectWidget)
{
	CreateServerButton->LevelPath = LevelSelectWidget->GetLevelPath();
	CreateServerButton->SetIsEnabled(true);
}

void UCreateServerUI::OnTextChanged(const FText& ServerName)
{
	FName Name = FName(ServerName.ToString());
	CreateServerButton->ServerName = Name;
}
