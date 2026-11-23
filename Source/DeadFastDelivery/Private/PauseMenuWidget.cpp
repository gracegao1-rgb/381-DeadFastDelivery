// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenuWidget.h"

#include "MyHUD.h"
#include "Kismet/GameplayStatics.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::HandleResume);
	}
	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::HandleRestart);
	}
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::HandleQuit);
	}
}

void UPauseMenuWidget::HandleResume() const
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AMyHUD* MyHUD = PC->GetHUD<AMyHUD>())
		{
			MyHUD->TogglePauseMenu();
		}
	}
}

void UPauseMenuWidget::HandleRestart() const
{
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName));
}

void UPauseMenuWidget::HandleQuit() const
{
	FString LevelName = TEXT("/Game/Levels/Lvl_MainMenu.Lvl_MainMenu");
	UGameplayStatics::OpenLevel(this, FName(*LevelName));
}
