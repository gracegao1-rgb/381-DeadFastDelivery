// Fill out your copyright notice in the Description page of Project Settings.


#include "ModalWidget.h"

#include "MyHUD.h"

void UModalWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ContinueButton)
	{
		ContinueButton->OnClicked.AddDynamic(this, &UModalWidget::HandleContinue);
	}
}

void UModalWidget::HandleContinue() const
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AMyHUD* MyHUD = PC->GetHUD<AMyHUD>())
		{
			MyHUD->HideInstructionsModal();
		}
	}
}
