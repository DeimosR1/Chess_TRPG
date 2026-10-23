// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuUI.generated.h"

/**
 * 
 */
UCLASS()
class UMainMenuUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(meta=(BindWidget))
	class UButton* JoinServerUIActivateButton;
	UPROPERTY(meta=(BindWidget))
	class UCreateServerMainMenuButton* CreateServerMainMenuButton;
	UPROPERTY(meta=(BindWidget))
	class UCreateServerUI* CreateServerUI;
	UPROPERTY(meta=(BindWidget))
	class UJoinServerUI* JoinServerUI;
	
	UFUNCTION()
	void OnJoinServerUIActivateButtonClicked();
	
public:
	UFUNCTION()
	void OnCreateServerUIActivateButtonClicked();
};
