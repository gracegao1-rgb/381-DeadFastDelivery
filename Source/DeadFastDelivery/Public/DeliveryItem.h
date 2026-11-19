// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeliveryItem.generated.h"

UCLASS()
class DEADFASTDELIVERY_API ADeliveryItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADeliveryItem();
	
	bool bIsDelivered = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Materials")
	class UMaterialInterface* GreenGlowMaterial;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Materials")
	class UMaterialInterface* RedGlowMaterial;
	
	// Delivery object's identifier should match the zone identifier
	UPROPERTY(EditAnywhere, Category = "Tuning")
	int ZoneIdentifier = 0;

public:	
	// what happens when this object enters the delivery zone
	void HandleStartZoneOverlap(int Zone);
	
	// what happens when this object leaves the delivery zone
	void HandleEndZoneOverlap();
};
