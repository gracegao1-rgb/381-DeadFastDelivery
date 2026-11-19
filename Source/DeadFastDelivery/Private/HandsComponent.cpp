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
	
	if (HandOffset.IsNearlyZero())
	{
		HandOffset = FVector(100.f, 0.f, 0.f);
	}

	if (!LeftHand)
	{
		LeftHand = CreateHand(TEXT("LeftHand"));
	}
	if (!RightHand)
	{
		RightHand = CreateHand(TEXT("RightHand"));
	}
}

// Helper method to create each hand
USphereComponent* UHandsComponent::CreateHand(const FName Name)
{
	AActor* Owner = GetOwner();
	if (!Owner) return nullptr;

	USphereComponent* Sphere = NewObject<USphereComponent>(Owner, Name);
	Sphere->InitSphereRadius(8.f);
	Sphere->SetupAttachment(nullptr);
	Sphere->RegisterComponent();
	
	// collision
	Sphere->SetCollisionProfileName(TEXT("PhysicsActor"));
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); // don't fight player capsule
	Sphere->IgnoreActorWhenMoving(Owner, true);

	Sphere->SetEnableGravity(false);
	Sphere->SetMassOverrideInKg(NAME_None, HandMassKg, true);
	Sphere->SetLinearDamping(2.f);
	Sphere->SetAngularDamping(20.f);
	Sphere->SetSimulatePhysics(true);
	
	Sphere->SetHiddenInGame(false);
	Sphere->ShapeColor = FColor::Red;
	return Sphere;
}

// Called every frame
void UHandsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) { return; }

	const FRotator ControlRot = OwnerPawn->GetControlRotation();
	const FVector Origin = OwnerPawn->GetPawnViewLocation();

	auto Place = [&](USphereComponent* Hand, const FVector& Pivot, const FRotator& HandRot, const FColor& Color)
	{
		if (!Hand) return;

		const FVector Desired = Origin + ControlRot.RotateVector(Pivot + HandRot.RotateVector(HandOffset));
		const FVector Current = Hand->GetComponentLocation();
		const FVector Error = Desired - Current;

		// If it lags too far behind (player moved quickly, or it got stuck), snap it back
		if (Error.SizeSquared() > FMath::Square(MaxHandLag))
		{
			Hand->SetWorldLocation(Desired, false, nullptr, ETeleportType::TeleportPhysics);
			Hand->SetPhysicsLinearVelocity(FVector::ZeroVector);
		}
		else
		{
			// Spring-damper drive, with the acceleration capped
			FVector Accel = Error * HandStiffness - Hand->GetPhysicsLinearVelocity() * HandDamping;
			Accel = Accel.GetClampedToMaxSize(HandMaxAccel);
			Hand->AddForce(Accel, NAME_None, /*bAccelChange*/ true);
		}
		UE_LOG(LogTemp, Warning, TEXT("%s sim=%d Err=%.1f Vel=%.1f Rot=%s"),
			*Hand->GetName(), Hand->IsSimulatingPhysics(), Error.Size(),
			Hand->GetPhysicsLinearVelocity().Size(), *HandRot.ToString());

		DrawDebugSphere(GetWorld(), Hand->GetComponentLocation(), 12.f, 12, Color, false, -1.f, 0, 1.f);
	};

	Place(LeftHand, LeftPivotLoc, LeftHandRot, FColor::Green);
	Place(RightHand, RightPivotLoc, RightHandRot, FColor::Red);
	
	UE_LOG(LogTemp, Warning, TEXT("HandOffset=%s RightRot=%s"), *HandOffset.ToString(), *RightHandRot.ToString());
}


void UHandsComponent::SetHandRotation(int HandOption, const FRotator& Rot)
{
	if (HandOption == 1)
	{
		// right hand
		RightHandRot = Rot;
	}
	if(HandOption == 2)
	{
		// left hand
		LeftHandRot = Rot;
	}
}

