// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "ModalWidget.h"
#include "PauseMenuWidget.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

/**
 * 
 */
UCLASS()
class DEADFASTDELIVERY_API AMyHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AMyHUD();
	
	virtual void Tick(float DeltaSeconds) override;
	
	void TogglePauseMenu();
	
	void HideInstructionsModal();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UHUDWidget> HUDWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPauseMenuWidget> PauseMenuWidgetClass;
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY() UHUDWidget* HUDWidget;
	
	UPROPERTY() UPauseMenuWidget* PauseMenuWidget;
	
	UPROPERTY() UModalWidget* InstructionsModalWidget;
	
	bool bIsPaused = false;
};
