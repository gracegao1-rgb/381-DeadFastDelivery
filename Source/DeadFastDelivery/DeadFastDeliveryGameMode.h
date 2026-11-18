// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DeadFastDeliveryGameMode.generated.h"

/**
 *  Simple GameMode for a first person game
 */
UCLASS(abstract)
class ADeadFastDeliveryGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADeadFastDeliveryGameMode();
	
	float GetTimeRemaining() const;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
	void UpdateCountdown();
	
	UPROPERTY(EditDefaultsOnly, Category = "Tuning", meta = (ClampMin = "0.0", Units = "s"))
	float LevelSeconds = 300.f;
	
	FTimerHandle LevelTimer;
};



