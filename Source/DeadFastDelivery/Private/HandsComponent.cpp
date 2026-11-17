// Fill out your copyright notice in the Description page of Project Settings.


#include "HandsComponent.h"

UHandsComponent::UHandsComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UHandsComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!LeftHand)
	{
		LeftHand  = CreateHand(TEXT("LeftHand"),  LeftHandRelativeLocation);
	}
	if (!RightHand)
	{
		RightHand = CreateHand(TEXT("RightHand"), RightHandRelativeLocation);
	}
}

// Helper method to create each hand
USphereComponent* UHandsComponent::CreateHand(const FName Name, const FVector& RelativeLocation)
{
	AActor* Owner = GetOwner();
	if (!Owner) { return nullptr; }

	USphereComponent* Sphere = NewObject<USphereComponent>(Owner, Name);
	Sphere->SetupAttachment(this);
	Sphere->InitSphereRadius(8.f);
	Sphere->SetRelativeLocation(RelativeLocation);
	Sphere->SetCollisionProfileName(TEXT("BlockAll"));
	Sphere->RegisterComponent();
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	// TODO: Temporary for now, replace with real hands and arms later
	Sphere->SetHiddenInGame(false);
	Sphere->ShapeColor = FColor::Red;
	Sphere->SetVisibility(true);
	
	return Sphere;
}


// Called every frame
void UHandsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (LeftHand)
	{
		const FVector Loc = LeftHand->GetComponentLocation();
		DrawDebugSphere(GetWorld(), Loc, 12.f, 12, FColor::Green, false, -1.f, 0, 1.f);
	}
	
	if (RightHand)
	{
		const FVector Loc = RightHand->GetComponentLocation();
		DrawDebugSphere(GetWorld(), Loc, 12.f, 12, FColor::Red, false, -1.f, 0, 1.f);
	}
	
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) { return; }

	const FRotator ControlRot = OwnerPawn->GetControlRotation();
	const FVector Origin = OwnerPawn->GetPawnViewLocation();
	const FVector DesiredLocLeft = Origin + ControlRot.RotateVector(LeftHandRelativeLocation + LeftHandRelativeOffset);
	const FVector DesiredLocRight = Origin + ControlRot.RotateVector(RightHandRelativeLocation + RightHandRelativeOffset);
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, FString::Printf(TEXT("Pitch: %f"), ControlRot.Pitch));
	
	if (LeftHand && RightHand)
	{
		LeftHand->SetWorldLocation(DesiredLocLeft, true);
		RightHand->SetWorldLocation(DesiredLocRight, true);
	}
}

