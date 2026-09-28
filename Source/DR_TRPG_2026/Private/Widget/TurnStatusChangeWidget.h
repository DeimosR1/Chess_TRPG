// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TurnStatusChangeWidget.generated.h"

/**
 * 
 */
UCLASS()
class UTurnStatusChangeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable)
	void PlayTurnChangeNotice(bool bIsPlayerTurn);
	
private:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* PlayerTurn;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* EnemyTurn;
	
	UPROPERTY(EditDefaultsOnly, Category="Turn")
	float DisplayDuration = 1.5f;

	FTimerHandle HideTimerHandle;

	UFUNCTION()
	void HideNotice();
	
};
