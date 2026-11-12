// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "MainMenuPageWidget.h"

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
		}
	}

	HandleSwitchPage(0);
}

void UMainMenuWidget::HandleSwitchPage(int32 TargetIndex)
{
	PageSwitcher->SetActiveWidget(PageSwitcher->GetWidgetAtIndex(TargetIndex));
}
