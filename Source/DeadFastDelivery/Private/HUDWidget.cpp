// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"

void UHUDWidget::SetTimeRemaining(int32 Seconds) const
{
	int32 const MinutesLeft = static_cast<int32>(Seconds / 60.0);
	int32 const SecondsLeft = static_cast<int32>(Seconds % 60);
	TimeText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), MinutesLeft, SecondsLeft)));
}

void UHUDWidget::SetDeliveredItems(int32 DeliveredItems, int32 TotalNumberOfItems) const
{
	DeliveredText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d Delivered"), DeliveredItems, TotalNumberOfItems)));
}


void UHUDWidget::SetDestroyedItems(int32 DestroyedItems) const
{
	if (DestroyedItems > 0)
		DestroyedText->SetText(FText::FromString(FString::Printf(TEXT("%d Destroyed"), DestroyedItems)));
	else
		DestroyedText->SetText(FText::FromString(FString::Printf(TEXT(""))));
}
