// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class ULevelSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	//Properties that hold the chosen Level
	UPROPERTY(EditDefaultsOnly)
	UWorld* HeldLevel;
	
private:
	UPROPERTY(meta=(BindWidget))
	class UButton* SelectButton;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* LevelName;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* LevelDescription;
	UPROPERTY(meta=(BindWidget))
	class UImage* LevelIcon;
	
};
