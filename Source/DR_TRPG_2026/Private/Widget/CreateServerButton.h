// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Multiplayer/MultiplayerGameLogicHandler.h"
#include "CreateServerButton.generated.h"

/**
 * 
 */
UCLASS()
class UCreateServerButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	//Properties that hold the chosen Level
	UPROPERTY(EditDefaultsOnly)
	FString LevelPath;
	UPROPERTY(EditDefaultsOnly)
	FName ServerName;
	
private:
	UPROPERTY(meta=(BindWidget))
	class UButton* ConfirmButton;
	
	UFUNCTION()
	void ConfirmButtonClicked();
	
	//Holds the Multiplayer Logic
	UPROPERTY(EditDefaultsOnly)
	UMultiplayerGameLogicHandler* MultiplayerJoiningHandler;
};
