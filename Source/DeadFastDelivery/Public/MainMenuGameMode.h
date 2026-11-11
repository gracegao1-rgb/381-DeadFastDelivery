// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainMenuWidget.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class DEADFASTDELIVERY_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMainMenuGameMode();
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY()
	TObjectPtr<UMainMenuWidget> MainMenuWidget;
};
