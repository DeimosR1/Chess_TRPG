// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelSelectWidget.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelSelected, class ULevelSelectWidget*, LevelSelectWidget);

UCLASS()
class ULevelSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Level")
	TSoftObjectPtr<UWorld> HeldLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Level")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Level")
	TObjectPtr<UTexture2D> Icon;

	FString GetLevelPath() const { return HeldLevel.ToSoftObjectPath().GetLongPackageName(); }

	FOnLevelSelected OnLevelSelected;

private:
	UPROPERTY(meta=(BindWidget)) 
	class UButton* SelectButton;
	UPROPERTY(meta=(BindWidget)) 
	class UTextBlock* LevelName;
	UPROPERTY(meta=(BindWidget)) 
	class UTextBlock* LevelDescription;
	UPROPERTY(meta=(BindWidget)) 
	class UImage* LevelIcon;

	UFUNCTION() void OnHandleClicked();
	
};
