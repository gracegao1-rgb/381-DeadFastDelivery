// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeadFastDeliveryGameMode.h"

#include "DeliveryItem.h"
#include "MyHUD.h"
#include "MyWorldSettings.h"
#include "Kismet/GameplayStatics.h"

ADeadFastDeliveryGameMode::ADeadFastDeliveryGameMode()
{
	// Set HUD
	HUDClass = AMyHUD::StaticClass();
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
	
	// Determine number of items to deliver
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADeliveryItem::StaticClass(), FoundActors);
	TotalNumberOfItems = FoundActors.Num();
}

void ADeadFastDeliveryGameMode::UpdateCountdown()
{
	if (LevelSeconds > 0.0f && !bPaused)
	{
		LevelSeconds -= 1;
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

float ADeadFastDeliveryGameMode::GetTimeRemaining() const
{
	return LevelSeconds;
}

int32 ADeadFastDeliveryGameMode::GetTotalNumberOfItems() const
{
	return TotalNumberOfItems;
}

int32 ADeadFastDeliveryGameMode::GetNumberOfDeliveredItems() const
{
	return NumberOfDeliveredItems;
}

int32 ADeadFastDeliveryGameMode::GetNumberOfDestroyedItems() const
{
	return NumberOfDestroyedItems;
}

void ADeadFastDeliveryGameMode::UpdateNumberOfDeliveredItems()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADeliveryItem::StaticClass(), FoundActors);
	
	int32 Count = 0;
	for (AActor* Actor : FoundActors)
	{
		ADeliveryItem* Item = Cast<ADeliveryItem>(Actor);
		if (Item->bIsDelivered)
		{
			Count++;
		}
	}
	NumberOfDeliveredItems = Count;
}

void ADeadFastDeliveryGameMode::IncrementNumberOfDestroyedItems()
{
	NumberOfDestroyedItems++;
}

void ADeadFastDeliveryGameMode::TogglePause()
{
	bPaused = !bPaused;
}
