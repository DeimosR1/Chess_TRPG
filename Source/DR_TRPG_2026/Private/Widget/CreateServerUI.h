// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreateServerUI.generated.h"

/**
 * 
 */
UCLASS()
class UCreateServerUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(meta=(BindWidget))
	TArray<class ULevelSelectWidget*> LevelSelectorWidgets;
	
	UPROPERTY(meta=(BindWidget))
	class UCreateServerButton* CreateServerButton;
};
