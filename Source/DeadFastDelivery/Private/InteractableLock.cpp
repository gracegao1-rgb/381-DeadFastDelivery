// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableLock.h"
#include "DeadFastDeliveryGameMode.h"

// Sets default values
AInteractableLock::AInteractableLock()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AInteractableLock::BeginPlay()
{
	Super::BeginPlay();
	
	if (UBoxComponent* Box = FindComponentByClass<UBoxComponent>())
	{
		Box->OnComponentBeginOverlap.AddDynamic(this, &AInteractableLock::HandleOverlap);
	}
}

void AInteractableLock::RunAway()
{
	FVector Origin = GetActorLocation(); // current location
	float Distance = 500.0f; // distance away fromg og location

	FVector RandomDirection = FMath::VRand();
	RandomDirection.Z = 0.0f;
	RandomDirection.Normalize();

	FVector NewLocation = Origin + (RandomDirection * Distance);
	
	EndLocation = NewLocation;
}

// Called every frame
// void AInteractableLock::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
// 	
// 	if (bUnlocked)
// 	{
// 		FVector NewLoc = FMath::VInterpTo(GetActorLocation(), EndLocation, DeltaTime, 2.f);
// 		SetActorLocation(NewLoc);
//
// 		if (NewLoc.Equals(EndLocation, 1.f))
// 		{
// 			SetActorLocation(EndLocation);
// 			bUnlocked = false;
// 		}
// 	}
// }

void AInteractableLock::Unlock()
{
	bUnlocked = true;
	DisableMeshCollision(); // can pass through gate
	
	GEngine->AddOnScreenDebugMessage(
	3,
	2.0f,
	FColor::Green, 
	TEXT("You can now pass through the gate!"));
}

void AInteractableLock::HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Sweep)
{
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (Pawn != nullptr && Pawn->IsPlayerControlled())
	{
		GEngine->AddOnScreenDebugMessage(
			3,
			2.0f,
			FColor::Green, 
			TEXT("Arrived at gate")        
		);
		
		if (ADeadFastDeliveryGameMode* Gm = GetWorld()->GetAuthGameMode<ADeadFastDeliveryGameMode>())
		{
			// RunAway(); // generate new location
			// report hatch reached
			Gm->ReportLockReached();
		}
	}
}

void AInteractableLock::DisableMeshCollision()
{
	if (bCollisionDisabled) return;
	bCollisionDisabled = true;

	if (IsValid(TargetMesh) && IsValid(BlockingMesh))
	{
		TargetMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BlockingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SignMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GEngine->AddOnScreenDebugMessage(
			3,
			2.0f,
			FColor::Green, 
			TEXT("disabled collision"));
	}
}