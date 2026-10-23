// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreateServerUI.generated.h"

class ULevelSelectWidget;
class UScrollBox;
/**
 * 
 */
UCLASS()
class UCreateServerUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	
private:
	UPROPERTY(EditAnywhere, Category="LevelSelect")
	TArray<TSubclassOf<ULevelSelectWidget>> LevelSelectorWidgets;
	
	UPROPERTY(meta=(BindWidget))
	class UCreateServerButton* CreateServerButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> LevelScrollBox;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;
	
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* ServerNameTextBox;
	
	UFUNCTION()
	void OnBackPressed();
	
	UFUNCTION()
	void LevelSelectedHandler(ULevelSelectWidget* LevelSelectWidget);
	
	UFUNCTION()
	void OnTextChanged(const FText& ServerName);
};
