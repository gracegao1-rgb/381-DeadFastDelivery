// Fill out your copyright notice in the Description page of Project Settings.


#include "HandsComponent.h"

#include "DeliveryItem.h"
#include "NiagaraTypes.h"
#include "Engine/OverlapResult.h"

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
	
	// Create Hands
	if (!LeftHand)
	{
		LeftHand  = CreateHand(TEXT("LeftHand"),  LeftHandRelativeLocation);
	}
	if (!RightHand)
	{
		RightHand = CreateHand(TEXT("RightHand"), RightHandRelativeLocation);
	}
	
	// Create Handles
	if (!LeftHandle)
	{
		LeftHandle  = CreateHandle();
	}
	if (!RightHandle)
	{
		RightHandle = CreateHandle();
	}
}

// Helper method to create each hand
USphereComponent* UHandsComponent::CreateHand(const FName Name, const FVector& HandRelativeLocation)
{
	AActor* Owner = GetOwner();
	if (!Owner) { return nullptr; }

	USphereComponent* Sphere = NewObject<USphereComponent>(Owner, Name);
	Sphere->SetupAttachment(this);
	Sphere->InitSphereRadius(8.f);
	Sphere->SetRelativeLocation(HandRelativeLocation);
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
	const FQuat ViewQuat = ControlRot.Quaternion();
	const FVector Origin = OwnerPawn->GetPawnViewLocation();

	// Rotate each hand's offset around its pivot (view-local), then move into world space
	const FVector DesiredLocLeft = Origin + ControlRot.RotateVector(
	LeftPivotLoc + LeftHandRot.RotateVector(LeftHandRelativeLocation + LeftHandRelativeOffset - LeftPivotLoc));

	const FVector DesiredLocRight = Origin + ControlRot.RotateVector(
	RightPivotLoc + RightHandRot.RotateVector(RightHandRelativeLocation + RightHandRelativeOffset - RightPivotLoc));

	if (LeftHand)
	{
		LeftHand->SetWorldLocation(DesiredLocLeft, true);
		LeftHand->SetWorldRotation(ViewQuat * LeftHandRot.Quaternion(), false);
	}

	if (RightHand)
	{
		RightHand->SetWorldLocation(DesiredLocRight, true);
		RightHand->SetWorldRotation(ViewQuat * RightHandRot.Quaternion(), false);
	}
	
	// Set location of grabbed object to hands with offset 
	if (LeftHandle && LeftHandle->GetGrabbedComponent() && LeftHand)
	{
		FTransform HandTransform = LeftHand->GetComponentTransform();
		HandTransform.SetScale3D(FVector::OneVector);
		const FTransform Target = LeftGrabOffset * HandTransform; // the side of the object, rather than its center
		LeftHandle->SetTargetLocationAndRotation(Target.GetLocation(), Target.Rotator());
	}
	if (RightHandle && RightHandle->GetGrabbedComponent() && RightHand)
	{
		FTransform HandTransform = RightHand->GetComponentTransform();
		HandTransform.SetScale3D(FVector::OneVector);
		const FTransform Target = RightGrabOffset * HandTransform;
		RightHandle->SetTargetLocationAndRotation(Target.GetLocation(), Target.Rotator());
	}
	
	UpdateGrip(LeftHandle, LeftHandGrip, DeltaTime);
	UpdateGrip(RightHandle, RightHandGrip, DeltaTime);
}

void UHandsComponent::UpdateGrip(UPhysicsHandleComponent* Handle, float& Grip, float DeltaTime)
{
	if (!Handle)
		return;
	
	// Decrease grip when grabbing object
	if (UMeshComponent* MeshComp = Cast<UMeshComponent>(Handle->GetGrabbedComponent()))
	{
		float Mass = MeshComp->GetMass();
		
		Grip -= 0.25 * DeltaTime;
		
		if (Grip < 0.f)
			Grip = 0.f;
	}
	// Replenish grip when not grabbing object
	else
	{
		Grip += 0.50 * DeltaTime;
		
		if (Grip > 1.f)
			Grip = 1.f;
	}
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

// Helper method to create each physics handle
UPhysicsHandleComponent* UHandsComponent::CreateHandle() const
{
	AActor* Owner = GetOwner();
	if (!Owner)
		return nullptr;

	UPhysicsHandleComponent* Handle = NewObject<UPhysicsHandleComponent>(Owner);
	Handle->bSoftAngularConstraint = true;
	Handle->bSoftLinearConstraint = true;
	Handle->bInterpolateTarget = true;
	Handle->LinearStiffness = 750.f;
	Handle->LinearDamping = 200.f;
	Handle->AngularStiffness = 1500.f;
	Handle->AngularDamping = 500.f;
	Handle->InterpolationSpeed = 50.f;
	Handle->RegisterComponent();
	return Handle;
}

void UHandsComponent::StartGrabbing()
{
	TryGrabbing(LeftHand, LeftHandle, LeftGrabOffset);
	TryGrabbing(RightHand, RightHandle, RightGrabOffset);
	
	GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Blue, 
			TEXT("Trying to grab")        
		);
}

void UHandsComponent::StopGrabbing()
{
	if (LeftHandle && LeftHandle->GetGrabbedComponent())
	{
		if (ADeliveryItem* Item = Cast<ADeliveryItem>(LeftHandle->GetGrabbedComponent()->GetOwner()))
		{
			Item->bIsBeingGrabbed = false;
		}
		LeftHandle->ReleaseComponent();
	}
	if (RightHandle && RightHandle->GetGrabbedComponent())
	{
		if (ADeliveryItem* Item = Cast<ADeliveryItem>(RightHandle->GetGrabbedComponent()->GetOwner()))
		{
			Item->bIsBeingGrabbed = false;
		}
		RightHandle->ReleaseComponent();
	}
	
	LeftHand->SetCollisionProfileName(TEXT("BlockAll"));
	RightHand->SetCollisionProfileName(TEXT("BlockAll"));
	
	GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Blue, 
			TEXT("Stopped Grabbing")        
		);
}

void UHandsComponent::TryGrabbing(USphereComponent* Hand, UPhysicsHandleComponent* Handle, FTransform& OutGrabOffset)
{
	if (!Hand || !Handle || Handle->GetGrabbedComponent())
		return;

	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(HandGrab), false, GetOwner());

	const FVector Center = Hand->GetComponentLocation();
	const float Reach = Hand->GetScaledSphereRadius() * 1.5f;

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(Overlaps, Center, FQuat::Identity,
		ObjParams, FCollisionShape::MakeSphere(Reach), Params);

	UPrimitiveComponent* Best = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();

	// filter for closest object
	for (const FOverlapResult& R : Overlaps)
	{
		UPrimitiveComponent* Comp = R.GetComponent();
		if (!Comp || !Comp->IsSimulatingPhysics())
			continue;

		const float DistSq = FVector::DistSquared(Center, Comp->GetComponentLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq; 
			Best = Comp;
		}
	}
	
	GEngine->AddOnScreenDebugMessage(
		-1,
		2.0f,
		FColor::Blue, 
		FString::Printf(TEXT("Overlap array length: %d"), Overlaps.Num())
	);

	if (Best)
	{
		FTransform HandTransform = Hand->GetComponentTransform();
		FTransform BestObjectTransform  = Best->GetComponentTransform();
		HandTransform.SetScale3D(FVector::OneVector);
		BestObjectTransform.SetScale3D(FVector::OneVector);
		
		// the current position of the hand, relative to the object getting grabbed
		OutGrabOffset = BestObjectTransform.GetRelativeTransform(HandTransform);
		
		// attach mesh to component
		Handle->GrabComponentAtLocationWithRotation(Best, NAME_None, BestObjectTransform.GetLocation(), BestObjectTransform.Rotator());
		
		// make the hands have no collision to make it easier to move around
		Hand->SetCollisionProfileName(TEXT("NoCollision"));
		
		// tell the item that they are being grabbed
		if (ADeliveryItem* Item = Cast<ADeliveryItem>(Best->GetOwner()))
		{
			Item->bIsBeingGrabbed = true;
		}
	}
}
