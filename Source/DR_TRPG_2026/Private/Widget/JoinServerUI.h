// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JoinServerUI.generated.h"

/**
 * 
 */
UCLASS()
class UJoinServerUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(meta=(BindWidget))
	class UButton* JoinButton;
	UPROPERTY(meta=(BindWidget))
	class UEditableTextBox* IPAdressInput;
	
};
