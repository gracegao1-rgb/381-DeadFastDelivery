// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "ModalWidget.generated.h"

/**
 * 
 */
UCLASS()
class DEADFASTDELIVERY_API UModalWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget)) UButton* ContinueButton;
	
	UFUNCTION() void HandleContinue() const;
};
