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
	
	int32 GetTotalNumberOfItems() const;
	
	int32 GetNumberOfDeliveredItems() const;
	
	int32 GetNumberOfDestroyedItems() const;
	
	void UpdateNumberOfDeliveredItems();
	
	void IncrementNumberOfDestroyedItems();
	
protected:
	virtual void BeginPlay() override;
	
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
	void UpdateCountdown();
	
	UPROPERTY(EditDefaultsOnly, Category = "Tuning", meta = (ClampMin = "0.0", Units = "s"))
	float LevelSeconds = 300.f;
	
	FTimerHandle LevelTimer;
	
	int32 TotalNumberOfItems = 0;
	
	int32 NumberOfDeliveredItems = 0;
	
	int32 NumberOfDestroyedItems = 0;
	
	// keys
	UPROPERTY(BlueprintReadOnly, Category = "Rules")
	int32 KeysCollected = 0;
	
public:
	/** A key was picked up. The key itself does not know what that means. */
	UFUNCTION(BlueprintCallable, Category = "Rules")
	void ReportKeyCollected();
	
	UFUNCTION(BlueprintCallable, Category = "Rules")
	void ReportLockReached();
	
	UFUNCTION(BlueprintPure, Category = "Rules")
	int32 GetKeysCollected() const { return KeysCollected; }
	
	UFUNCTION(BlueprintPure, Category = "Rules")
	float GetCurrentScore() const { return TotalScore; }
	
	UFUNCTION(BlueprintPure, Category = "Rules")
	float GetMaxScore() const { return MaxScore; }
	
	float MaxScore = 0; // maximum possible score -- changes as objects break
	
	float TotalScore = 0; // currently owned score
	
	float MinPassingScore = 200;
	
	void ReportMaxCost(float Cost);

	void ReportCost(float Cost);
	
	bool bGameStarted = false;
};



