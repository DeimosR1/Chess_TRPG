// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerListSegment.generated.h"

/**
 * 
 */
UCLASS()
class UServerListSegment : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY() 
	FString PureTextServerName;
	UPROPERTY() 
	int32 SessionIndex = INDEX_NONE;
	
	UFUNCTION()
	void SetDisplayName();
	
private:
	
	UPROPERTY(meta=(BindWidget)) 
	class UButton* ConfirmButton;
	UPROPERTY(meta=(BindWidget)) 
	class UTextBlock* ServerName;
	
	UFUNCTION()
	void OnConfirmClicked();
	
	UPROPERTY(EditDefaultsOnly)
	class UMultiplayerGameLogicHandler* MultiplayerJoiningHandler;

};
