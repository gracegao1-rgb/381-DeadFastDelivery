// Fill out your copyright notice in the Description page of Project Settings.


#include "DeliveryItem.h"

// Sets default values
ADeliveryItem::ADeliveryItem()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DeliveryItemMesh"));
	RootComponent = Mesh;
	
	// Setup physics
	if (Mesh)
	{
		Mesh->SetMobility(EComponentMobility::Movable);
		Mesh->SetSimulatePhysics(true);
		Mesh->SetEnableGravity(true);
	}
	
	// Get material assets
	ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("/Game/Materials/M_GlowingAura.M_GlowingAura"));
	if (MaterialAsset.Succeeded())
	{
		GreenGlowMaterial = MaterialAsset.Object;
	}
}

// Called when the game starts or when spawned
void ADeliveryItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADeliveryItem::HandleStartZoneOverlap(int Zone)
{
	if (Zone == ZoneIdentifier)
	{
		bIsDelivered = true;
		if (Mesh && GreenGlowMaterial)
		{
			Mesh->SetOverlayMaterial(GreenGlowMaterial);
		}
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,TEXT("Object delivered!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,TEXT("Object delivered to wrong zone!"));
	}
}

void ADeliveryItem::HandleEndZoneOverlap()
{
	bIsDelivered = false;
	if (Mesh)
	{
		Mesh->SetOverlayMaterial(nullptr);
	}
}

