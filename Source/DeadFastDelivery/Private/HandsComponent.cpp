// Fill out your copyright notice in the Description page of Project Settings.


#include "HandsComponent.h"

#include "DeadFastDeliveryCharacter.h"
#include "DeliveryItem.h"
#include "NiagaraTypes.h"
#include "Engine/OverlapResult.h"
#include "Materials/MaterialInstanceDynamic.h"

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
		LeftHand  = CreateHand(TEXT("LeftHand"),  LeftHandRelativeLocation, LeftArmSegment);
	}
	if (!RightHand)
	{
		RightHand = CreateHand(TEXT("RightHand"), RightHandRelativeLocation, RightArmSegment);
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
USphereComponent* UHandsComponent::CreateHand(const FName Name, const FVector& HandRelativeLocation, TObjectPtr<UStaticMeshComponent>& OutArmSegment)
{
	AActor* Owner = GetOwner();
	if (!Owner) { return nullptr; }
	
	// create arm
	OutArmSegment = NewObject<UStaticMeshComponent>(Owner, FName(*FString::Printf(TEXT("%s_Arm"), *Name.ToString())));
	OutArmSegment->SetupAttachment(Owner->GetRootComponent());
	OutArmSegment->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OutArmSegment->SetCastShadow(false);
	if (UStaticMesh* Arm = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Fab/lowpoly_arm/StaticMeshes/Armp__0.Armp__0")))
	{
		OutArmSegment->SetStaticMesh(Arm);
	}
	OutArmSegment->RegisterComponent();
	
	// set arm color
	UMaterialInterface* Base = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/gltf/Textures/M_Green.M_Green"));
	if (Base)
	{
		OutArmSegment->SetMaterial(0, Base);
	}

	// create hand
	USphereComponent* Sphere = NewObject<USphereComponent>(Owner, Name);
	Sphere->SetupAttachment(this);
	Sphere->InitSphereRadius(8.f);
	Sphere->SetRelativeLocation(HandRelativeLocation);
	Sphere->SetCollisionProfileName(TEXT("BlockAll"));
	OutArmSegment->SetCollisionResponseToAllChannels(ECR_Ignore);
	OutArmSegment->SetGenerateOverlapEvents(false);
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
	
	// arm rotation
	const FQuat ArmViewQuat = OwnerPawn->GetControlRotation().Quaternion();
	const FVector ArmOrigin = OwnerPawn->GetPawnViewLocation();

	UpdateArm(LeftArmSegment,  LeftHand,  LeftShoulderLocal,  Origin, ViewQuat, true);
	UpdateArm(RightArmSegment, RightHand, RightShoulderLocal, Origin, ViewQuat, false);
	
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
	
	UpdateGrip(LeftHandle, RightHandle, LeftHandGrip, DeltaTime);
	UpdateGrip(RightHandle, LeftHandle, RightHandGrip, DeltaTime);
}

void UHandsComponent::UpdateGrip(UPhysicsHandleComponent* Handle, UPhysicsHandleComponent* OtherHandle, float& Grip, float DeltaTime)
{
	if (!Handle)
		return;
	
	// Decrease grip when grabbing object
	if (UMeshComponent* MeshComp = Cast<UMeshComponent>(Handle->GetGrabbedComponent()))
	{
		float Mass = MeshComp->GetMass();
		float Modifier = 1.f;
		
		if (OtherHandle && OtherHandle->GetGrabbedComponent())
		{
			UMeshComponent* OtherMeshComp = Cast<UMeshComponent>(Handle->GetGrabbedComponent());
			if (OtherMeshComp == MeshComp) // both hands are grabbing the same object
				Modifier = 0.5; 
		}
		
		Grip -= Mass * (0.05/50) * DeltaTime * Modifier; // one hand can hold a 50kg object for 20 seconds, scales by weight
		
		if (Grip < 0.f)
		{
			Grip = 0.f;
			Handle->ReleaseComponent();
		}
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

void UHandsComponent::UpdateArm(UStaticMeshComponent* Arm, USphereComponent* Hand,
								const FVector& ShoulderLocal,
								const FVector& Origin, const FQuat& ViewQuat, bool bMirror)
{
	if (!Arm || !Hand) return;

	const FVector Shoulder = Origin + ViewQuat.RotateVector(ShoulderLocal);
	const FVector HandPos  = Hand->GetComponentLocation();

	const FVector Dir = HandPos - Shoulder;
	if (Dir.SizeSquared() < KINDA_SMALL_NUMBER) return;

	const FQuat Offset = FRotator(-60.f, 180.f, 280.f).Quaternion();
	const float S = 0.5f;

	FQuat Final;
	FVector Scale(S);

	if (!bMirror)
	{
		Final = FRotationMatrix::MakeFromXZ(Dir, ViewQuat.GetUpVector()).ToQuat() * Offset;
	}
	else
	{
		// Reflect across the view's center plane (normal = view right)
		const FVector N = ViewQuat.GetAxisY();
		auto Reflect = [&N](const FVector& V) { return V - 2.f * FVector::DotProduct(V, N) * N; };

		// Build the right arm's orientation for the mirrored direction
		const FQuat RightFinal =
			FRotationMatrix::MakeFromXZ(Reflect(Dir), ViewQuat.GetUpVector()).ToQuat() * Offset;

		// Reflect its axes, then negate local Z (matches the Z flip in Scale) to get a valid rotation
		const FVector X = Reflect(RightFinal.GetAxisX());
		const FVector Y = Reflect(RightFinal.GetAxisY());
		const FVector Z = Reflect(RightFinal.GetAxisZ());
		Final = FQuat(FMatrix(X, Y, -Z, FVector::ZeroVector));

		Scale = FVector(S, S, -S);
	}

	FVector LocalCenter = FVector::ZeroVector;
	if (const UStaticMesh* Mesh = Arm->GetStaticMesh())
	{
		LocalCenter = Mesh->GetBounds().Origin;
	}

	const FVector Mid = (Shoulder + HandPos) * 0.5f;

	Arm->SetWorldRotation(Final);
	Arm->SetWorldScale3D(Scale);
	Arm->SetWorldLocation(Mid - Final.RotateVector(LocalCenter * Scale));
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
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	if (LeftArmSegment)  Params.AddIgnoredComponent(LeftArmSegment.Get());
	if (RightArmSegment) Params.AddIgnoredComponent(RightArmSegment.Get());
	
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
	const float Reach = Hand->GetScaledSphereRadius() * 2.f;

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
