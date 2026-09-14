// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/OverheadHealthStatus.h"

#include "ValueGauge.h"

void UOverheadHealthStatus::SetValueOfHealthBar(int32 Value, int32 MaxValue)
{
	HealthBar->SetValue(Value, MaxValue);
}
