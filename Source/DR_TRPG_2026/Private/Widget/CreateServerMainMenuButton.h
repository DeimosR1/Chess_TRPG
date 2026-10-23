// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreateServerMainMenuButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCreateServerMainMenuButtonClicked);
/**
 * 
 */
UCLASS()
class UCreateServerMainMenuButton : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(meta=(BindWidget))
	class UButton* CreateServerMainMenuButton;
	
	UFUNCTION()
	void OnCreateServerMainMenuButtonClicked();
	
public:
	FOnCreateServerMainMenuButtonClicked OnClickedCreate;
};
