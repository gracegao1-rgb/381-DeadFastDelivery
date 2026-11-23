// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class DEADFASTDELIVERY_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetTimeRemaining(int32 Seconds) const;
	void SetDeliveredItems(int32 DeliveredItems, int32 TotalNumberOfItems) const;
	void SetDestroyedItems(int32 DestroyedItems) const;
	void SetLeftGripBar(float LeftHandGrip) const;
	void SetRightGripBar(float LeftHandGrip) const;
	
protected:
	UPROPERTY(meta = (BindWidget)) UTextBlock* TimeText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* DeliveredText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* DestroyedText;
	UPROPERTY(meta = (BindWidget)) UProgressBar* LeftGripBar;
	UPROPERTY(meta = (BindWidget)) UProgressBar* RightGripBar;
	
};
