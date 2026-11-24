// Fill out your copyright notice in the Description page of Project Settings.


#include "SlipperyPuddle.h"

#include "DeadFastDeliveryCharacter.h"

// Sets default values
ASlipperyPuddle::ASlipperyPuddle()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// Create cylinder collision volume
	CylinderZone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CylinderZone"));
	SetRootComponent(CylinderZone);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder")); // radius 50, height 100
	if (CylinderMesh.Succeeded())
	{
		CylinderZone->SetStaticMesh(CylinderMesh.Object);
	}
	CylinderZone->SetRelativeScale3D(FVector(8.f, 8.f, 1.f)); // radius 400, height 100

	CylinderZone->SetVisibility(false);
	CylinderZone->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CylinderZone->SetCollisionProfileName(TEXT("OverlapAllDynamic")); 
	CylinderZone->SetGenerateOverlapEvents(true);
	
	// Add visible puddle mesh
	PuddleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PuddleMesh"));
	PuddleMesh->SetupAttachment(CylinderZone);

	if (CylinderMesh.Succeeded())
	{
		PuddleMesh->SetStaticMesh(CylinderMesh.Object);
	}

	PuddleMesh->SetRelativeScale3D(FVector(1.f, 1.f, 0.1f)); // height 10
	PuddleMesh->SetRelativeLocation(FVector(0.f, 0.f, -45.f)); // on the floor

	PuddleMesh->SetVisibility(true);
	PuddleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PuddleMesh->SetGenerateOverlapEvents(false);
}

// Called when the game starts or when spawned
void ASlipperyPuddle::BeginPlay()
{
	Super::BeginPlay();
	
	CylinderZone->OnComponentBeginOverlap.AddDynamic(this, &ASlipperyPuddle::OnBoxOverlapBegin);
	CylinderZone->OnComponentEndOverlap.AddDynamic(this, &ASlipperyPuddle::OnBoxOverlapEnd);
}


void ASlipperyPuddle::OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(ADeadFastDeliveryCharacter::StaticClass()))
	{
		ADeadFastDeliveryCharacter* Character = Cast<ADeadFastDeliveryCharacter>(OtherActor);
		Character->EnterPuddle();
	}
	
}

void ASlipperyPuddle::OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA(ADeadFastDeliveryCharacter::StaticClass()))
	{
		ADeadFastDeliveryCharacter* Character = Cast<ADeadFastDeliveryCharacter>(OtherActor);
		Character->ExitPuddle();
	}
}


