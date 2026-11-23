// Fill out your copyright notice in the Description page of Project Settings.


#include "DeliveryItem.h"

#include "DeadFastDeliveryCharacter.h"
#include "DeadFastDeliveryGameMode.h"
#include "HandsComponent.h"

// Sets default values
ADeliveryItem::ADeliveryItem()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DeliveryItemMesh"));
	RootComponent = Mesh;
	
	// Get material assets
	ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("/Game/Materials/M_GlowingAura.M_GlowingAura"));
	if (MaterialAsset.Succeeded())
	{
		GreenGlowMaterial = MaterialAsset.Object;
	}
	ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset2(TEXT("/Game/Materials/M_GlowingAuraRed.M_GlowingAuraRed"));
	if (MaterialAsset2.Succeeded())
	{
		RedGlowMaterial = MaterialAsset2.Object;
	}
}

// Called when the game starts or when spawned
void ADeliveryItem::BeginPlay()
{
	Super::BeginPlay();
	
	// Setup physics
	if (Mesh)
	{
		Mesh->SetMobility(EComponentMobility::Movable);
		Mesh->SetSimulatePhysics(true);
		Mesh->SetEnableGravity(true);
		Mesh->OnComponentHit.AddDynamic(this, &ADeliveryItem::HandleHit);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetNotifyRigidBodyCollision(true);
	}
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
		
		if (ADeadFastDeliveryGameMode* Gm = GetWorld()->GetAuthGameMode<ADeadFastDeliveryGameMode>())
		{
			Gm->UpdateNumberOfDeliveredItems();
		}
	}
	else
	{
		if (Mesh && RedGlowMaterial)
		{
			Mesh->SetOverlayMaterial(RedGlowMaterial);
		}
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
	if (ADeadFastDeliveryGameMode* Gm = GetWorld()->GetAuthGameMode<ADeadFastDeliveryGameMode>())
	{
		Gm->UpdateNumberOfDeliveredItems();
	}
}


void ADeliveryItem::HandleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Ignore hitting ourselves, our owner, or hands
	if (OtherActor == this || OtherActor == GetOwner())
		return;
	if (OtherActor && (OtherActor->IsA<ADeadFastDeliveryCharacter>() || OtherActor->IsA<UHandsComponent>()))
		return;

	if (!HasDurability)
		return;
	if (bIsBeingGrabbed) // don't apply damage when being grabbed
		return;
	
	UClass* ActorClass = OtherActor->GetClass();
	float ImpactForce = NormalImpulse.Size()/Mesh->GetMass()/100; // Newton Seconds
	if (ImpactForce > 5.0f)
	{
		Health -= ImpactForce;
		if (Health <= 0.0f)
			HandleDestroy();
		
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,FString::Printf(TEXT("Object hit! %s"), *ActorClass->GetName()));
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,FString::Printf(TEXT("Health Remaining! %f"), Health));
	}
}

void ADeliveryItem::HandleDestroy()
{
	Destroy();
}

