#include "MainMenuGameMode.h"
#include "MainMenuWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	// no character in menu
	DefaultPawnClass = nullptr;
	
	HUDClass = AHUD::StaticClass();

	// find the widget blueprint
	static ConstructorHelpers::FClassFinder<UMainMenuWidget> MenuWidgetBP(
		TEXT("/Game/Blueprints/UI/WBP_MainMenu"));

	if (MenuWidgetBP.Succeeded())
	{
		MainMenuWidgetClass = MenuWidgetBP.Class;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MainMenuGameMode: could not find widget"));
	}
}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC || !MainMenuWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("MainMenuGameMode: missing PlayerController or MainMenuWidgetClass"));
		return;
	}
	
	// find camera
	TArray<AActor*> Cameras;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), Cameras);

	if (Cameras.Num() > 0)
	{
		PC->SetViewTargetWithBlend(Cameras[0], 0.f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MainMenuGameMode: can't find camera"));
	}

	// add UI widget
	MainMenuWidget = CreateWidget<UMainMenuWidget>(PC, MainMenuWidgetClass);
	if (MainMenuWidget)
	{
		MainMenuWidget->AddToViewport();

		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);
	}
}