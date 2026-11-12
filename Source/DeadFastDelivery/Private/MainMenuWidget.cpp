// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "MainMenuPageWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!PageSwitcher) return;

	// subscribe to every page
	for (UWidget* Child : PageSwitcher->GetAllChildren())
	{
		if (UMainMenuPageWidget* Page = Cast<UMainMenuPageWidget>(Child))
		{
			Page->OnSwitchPage.AddDynamic(this, &UMainMenuWidget::HandleSwitchPage);
			Page->OnAction.AddDynamic(this, &UMainMenuWidget::HandleAction);
		}
	}

	HandleSwitchPage(0);
}

void UMainMenuWidget::HandleSwitchPage(int32 TargetIndex)
{
	PageSwitcher->SetActiveWidget(PageSwitcher->GetWidgetAtIndex(TargetIndex));
}

void UMainMenuWidget::HandleAction(EMenuActions Action)
{
	switch (Action)
	{
	case EMenuActions::QuitGame:
		QuitGame();
		break;
	case EMenuActions::LoadLevelTutorial:
		LoadLevel(TEXT("/Game/Levels/Lvl_Tutorial"));
		break;
	case EMenuActions::LoadLevelMet:
		LoadLevel(TEXT("/Game/Levels/Lvl_THE_MET"));
		break;
	default:
		break;
	}
}


void UMainMenuWidget::QuitGame() const
{
	UKismetSystemLibrary::QuitGame(
		GetWorld(),
		nullptr, 
		EQuitPreference::Quit,
		false
	);
}

void UMainMenuWidget::LoadLevel(FName LevelName) const
{
	UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}
