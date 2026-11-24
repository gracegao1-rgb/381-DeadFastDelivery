// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"

#include "DeadFastDeliveryCharacter.h"
#include "DeadFastDeliveryGameMode.h"
#include "MyWorldSettings.h"

AMyHUD::AMyHUD()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// adds widget blueprint to HUD class
	static ConstructorHelpers::FClassFinder<UHUDWidget> WidgetBPClass(
		TEXT("/Game/Blueprints/UI/WBP_HUD"));
	if (WidgetBPClass.Succeeded())
	{
		HUDWidgetClass = WidgetBPClass.Class;
	}
	
	static ConstructorHelpers::FClassFinder<UPauseMenuWidget> PauseWidgetBPClass(
	TEXT("/Game/Blueprints/UI/WBP_PauseMenu"));
	if (PauseWidgetBPClass.Succeeded())
	{
		PauseMenuWidgetClass = PauseWidgetBPClass.Class;
	}
}

void AMyHUD::BeginPlay()
{
	Super::BeginPlay();

	// creates and adds widget blueprint to HUD UI
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UHUDWidget>(GetOwningPlayerController(), HUDWidgetClass);
		if (HUDWidget) HUDWidget->AddToViewport();
	}
	
	// create pause menu widget, but doesn't add it
	if (PauseMenuWidgetClass)
	{
		PauseMenuWidget = CreateWidget<UPauseMenuWidget>(GetOwningPlayerController(), PauseMenuWidgetClass);
		if (PauseMenuWidget)
		{
			PauseMenuWidget->AddToViewport(10);
			PauseMenuWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	
	// get level instructions modal from world settings
	UWorld* World = GetWorld();
	if (!World) return;

	const AMyWorldSettings* WS = Cast<AMyWorldSettings>(World->GetWorldSettings());
	if (WS && WS->LevelInstructionsModal)
	{
		if (APlayerController* PC = GetOwningPlayerController())
		{
			InstructionsModalWidget = CreateWidget<UModalWidget>(PC, WS->LevelInstructionsModal);
			if (InstructionsModalWidget)
			{
				InstructionsModalWidget->AddToViewport();
				
				FInputModeGameAndUI InputMode;
				InputMode.SetWidgetToFocus(InstructionsModalWidget->TakeWidget());
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

				PC->SetInputMode(InputMode);
				PC->SetShowMouseCursor(true);
				PC->SetIgnoreLookInput(true);
			}
		}
	}
}

void AMyHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HUDWidget) return;
	if (const ADeadFastDeliveryGameMode* Gm = GetWorld()->GetAuthGameMode<ADeadFastDeliveryGameMode>())
	{
		HUDWidget->SetTimeRemaining(Gm->GetTimeRemaining());
		HUDWidget->SetDeliveredItems(Gm->GetNumberOfDeliveredItems(), Gm->GetTotalNumberOfItems());
		HUDWidget->SetDestroyedItems(Gm->GetNumberOfDestroyedItems());
	}
	
	if (APlayerController* PC = GetOwningPlayerController())
	{
		if (ADeadFastDeliveryCharacter* Character = Cast<ADeadFastDeliveryCharacter>(PC->GetPawn()))
		{
			HUDWidget->SetLeftGripBar(Character->GetLeftHandGrip());
			HUDWidget->SetRightGripBar(Character->GetRightHandGrip());
		}
	}
}

void AMyHUD::TogglePauseMenu()
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC) return;
	
	if (!bIsPaused)
	{
		if (PauseMenuWidget)
			PauseMenuWidget->SetVisibility(ESlateVisibility::Visible);
		bIsPaused = true;
		
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);
		PC->SetIgnoreLookInput(true);
		PC->SetPause(true);
	}
	else
	{
		if (PauseMenuWidget)
			PauseMenuWidget->SetVisibility(ESlateVisibility::Hidden);
		bIsPaused = false;
		
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
		PC->SetIgnoreLookInput(false);
		PC->SetPause(false);
	}
}

void AMyHUD::HideInstructionsModal()
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC) return;
	
	PC->SetInputMode(FInputModeGameOnly());
	PC->SetShowMouseCursor(false);
	PC->SetIgnoreLookInput(false);
	
	if (InstructionsModalWidget)
		InstructionsModalWidget->RemoveFromParent();
}
