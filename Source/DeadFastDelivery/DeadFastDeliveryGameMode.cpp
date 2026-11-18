// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeadFastDeliveryGameMode.h"

#include "MyWorldSettings.h"

ADeadFastDeliveryGameMode::ADeadFastDeliveryGameMode()
{
	// stub
}

// Import per-level settings from WorldSettings
void ADeadFastDeliveryGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	if (const AMyWorldSettings* WS = Cast<AMyWorldSettings>(GetWorldSettings()))
	{
		LevelSeconds = WS->LevelSeconds;
	}
}

void ADeadFastDeliveryGameMode::BeginPlay()
{
	// Start timer
	GetWorldTimerManager().SetTimer(LevelTimer, this, &ADeadFastDeliveryGameMode::UpdateCountdown, 1.0f, true);
}

void ADeadFastDeliveryGameMode::UpdateCountdown()
{
	if (LevelSeconds > 0.0f)
	{
		LevelSeconds -= 1;
		
		FString TimeMessage = FString::Printf(TEXT("Current Time: %f"), LevelSeconds);
		GEngine->AddOnScreenDebugMessage(
			1,
			2.0f,
			FColor::Green, 
			TimeMessage        
		);
		
	}
	else
	{
		// TODO: end game
		
		GEngine->AddOnScreenDebugMessage(
			1,
			2.0f,
			FColor::Blue, 
			TEXT("Timer Ended")        
		);
	}
}