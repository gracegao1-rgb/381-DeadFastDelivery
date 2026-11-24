// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModalWidget.h"
#include "GameFramework/WorldSettings.h"
#include "MyWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class DEADFASTDELIVERY_API AMyWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Tuning", meta = (ClampMin = "0.0", Units = "s"))
	float LevelSeconds = 300.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tuning")
	TSubclassOf<UModalWidget> LevelInstructionsModal; // Showed at the start of the level
};
