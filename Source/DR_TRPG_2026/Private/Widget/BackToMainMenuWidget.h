// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BackToMainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class UBackToMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
private:
	
	UPROPERTY(meta=(BindWidget))
	class UButton* BackToMainMenuButton;
	UPROPERTY(meta=(BindWidget))
	class UButton* CancelOperation;
	
	UFUNCTION()
	void OnBackToMainClicked();
	UFUNCTION()
	void OnCancelOperationClicked();
	
};
