// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "JoinServerUI.generated.h"

class UServerListSegment;
/**
 * 
 */
UCLASS()
class UJoinServerUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(EditAnywhere, Category="ServerSelect")
	TSubclassOf<UServerListSegment> ServerListItemBase;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* SearchButton;
	UPROPERTY(meta=(BindWidget)) 
	class UScrollBox* ServerListScrollBox;
	UPROPERTY(meta=(BindWidget))
	class UButton* BackButton;
	
	UFUNCTION()
	void SearchForServers();
	UFUNCTION()
	void BackButtonClicked();
	UFUNCTION()
	void OnServersFound(bool bSuccess);
	
	//Holds the Multiplayer Logic
	UPROPERTY()
	class UMultiplayerGameLogicHandler* MultiplayerJoiningHandler;
	
};
