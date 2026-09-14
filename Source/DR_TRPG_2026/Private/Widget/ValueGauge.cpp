// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/ValueGauge.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UValueGauge::NativePreConstruct()
{
	Super::NativePreConstruct();
	ProgressBar->SetFillColorAndOpacity(BarColor);
}

void UValueGauge::SetValue(float NewValue, float NewMaxValue)
{
	CachedValue = NewValue;
	CachedMaxValue = NewMaxValue;
	if (NewMaxValue == 0) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Max value cannot be 0 for the value gauge!!"))
		return;
	}

	ProgressBar->SetPercent(NewValue / NewMaxValue);

	static FNumberFormattingOptions FormattingOptions = FNumberFormattingOptions();

	ValueText->SetText(FText::Format(FTextFormat::FromString("{0}/{1}"), FText::AsNumber(NewValue, &FormattingOptions), FText::AsNumber(NewMaxValue, &FormattingOptions)));
}

void UValueGauge::ValueChanged(int32 ChangedData)
{
	SetValue(ChangedData, CachedMaxValue);
}

void UValueGauge::MaxValueChanged(int32 ChangedData)
{
	SetValue(CachedValue, ChangedData);
}
