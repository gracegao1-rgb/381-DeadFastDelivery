#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuPageWidget.generated.h"

UENUM(BlueprintType)
enum EMenuActions : uint8
{
	QuitGame,
	LoadLevelTutorial,
	LoadLevelMet
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchPage, int32, TargetIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAction, EMenuActions, Action);

UCLASS()
class DEADFASTDELIVERY_API UMainMenuPageWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Call from a button's OnClicked in the page Blueprint. */
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void RequestSwitchPage(int32 TargetIndex) { OnSwitchPage.Broadcast(TargetIndex); }

	/** Call from a button's OnClicked in the page Blueprint. */
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void RequestAction(EMenuActions Action) { OnAction.Broadcast(Action); }
	
	FOnSwitchPage OnSwitchPage;
	FOnAction OnAction;
};
