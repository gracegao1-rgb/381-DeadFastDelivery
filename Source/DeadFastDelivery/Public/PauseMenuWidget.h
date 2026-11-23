// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "PauseMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class DEADFASTDELIVERY_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget)) UButton* ResumeButton;
	UPROPERTY(meta = (BindWidget)) UButton* RestartButton;
	UPROPERTY(meta = (BindWidget)) UButton* QuitButton;
	
	UFUNCTION()
	void HandleResume() const;
	
	UFUNCTION()
	void HandleRestart() const;
	
	UFUNCTION()
	void HandleQuit() const;
};
