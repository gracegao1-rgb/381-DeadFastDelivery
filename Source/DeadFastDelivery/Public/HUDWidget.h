// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	
protected:
	UPROPERTY(meta = (BindWidget)) UTextBlock* TimeText;
	
};
