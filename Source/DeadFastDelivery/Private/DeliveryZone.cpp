// Fill out your copyright notice in the Description page of Project Settings.


#include "DeliveryZone.h"

#include "DeliveryItem.h"

// Sets default values
ADeliveryZone::ADeliveryZone()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Zone = CreateDefaultSubobject<UBoxComponent>(TEXT("DeliveryZone"));
	RootComponent = Zone;
	Zone->SetBoxExtent(FVector(300.f, 300.f, 300.f));
	Zone->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

// Called when the game starts or when spawned
void ADeliveryZone::BeginPlay()
{
	Super::BeginPlay();
	
	Zone->OnComponentBeginOverlap.AddDynamic(this, &ADeliveryZone::OnBoxOverlapBegin);
	Zone->OnComponentEndOverlap.AddDynamic(this, &ADeliveryZone::OnBoxOverlapEnd);
}

void ADeliveryZone::OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(ADeliveryItem::StaticClass()))
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Blue, 
			TEXT("Object began overlapping")        
		);
		
		ADeliveryItem* Item = static_cast<ADeliveryItem*>(OtherActor);
		Item->HandleStartZoneOverlap(ZoneIdentifier);
	}
	
}

void ADeliveryZone::OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA(ADeliveryItem::StaticClass()))
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Blue, 
			TEXT("Object stopped overlapping")        
		);
		
		ADeliveryItem* Item = static_cast<ADeliveryItem*>(OtherActor);
		Item->HandleEndZoneOverlap();
	}
	
}



