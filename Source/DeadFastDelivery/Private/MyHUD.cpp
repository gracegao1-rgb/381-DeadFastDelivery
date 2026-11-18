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
}

void AMyHUD::BeginPlay()
{
	Super::BeginPlay();

	// adds widget blueprint to HUD UI
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UHUDWidget>(GetOwningPlayerController(), HUDWidgetClass);
		if (HUDWidget) HUDWidget->AddToViewport();
	}
}

void AMyHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HUDWidget) return;
	if (const ADeadFastDeliveryGameMode* Gm = GetWorld()->GetAuthGameMode<ADeadFastDeliveryGameMode>())
	{
		HUDWidget->SetTimeRemaining(Gm->GetTimeRemaining());
	}
}
