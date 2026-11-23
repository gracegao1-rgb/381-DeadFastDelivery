// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"

#include "DeadFastDeliveryGameMode.h"

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
	
	static ConstructorHelpers::FClassFinder<UHUDWidget> PauseWidgetBPClass(
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
		if (PauseMenuWidget) PauseMenuWidget->AddToViewport(10);
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
}

void AMyHUD::TogglePauseMenu() const
{
	if (!bIsPaused)
	{
		if (PauseMenuWidget)
			PauseMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		if (PauseMenuWidget)
			PauseMenuWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}
