// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadHealthStatus.generated.h"

/**
 * 
 */
UCLASS()
class UOverheadHealthStatus : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void SetValueOfHealthBar(int32 Value, int32 MaxValue);
	
private:
	UPROPERTY(meta = (BindWidget))
	class UValueGauge* HealthBar;
};
