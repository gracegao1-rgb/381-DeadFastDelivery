// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeliveryItem.generated.h"

class UNiagaraComponent;

UCLASS()
class DEADFASTDELIVERY_API ADeliveryItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADeliveryItem();
	
	bool bIsDelivered = false;
	
	bool bIsBeingGrabbed = false;
	
	bool bIsDestroyed = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void HandleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	// Called when item breaks
	void HandleDestroy();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Materials")
	class UMaterialInterface* GreenGlowMaterial;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Materials")
	class UMaterialInterface* RedGlowMaterial;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> ExplosionEffect;
	
	// Delivery object's identifier should match the zone identifier
	UPROPERTY(EditAnywhere, Category = "Tuning")
	int ZoneIdentifier = 0;
	
	UPROPERTY(EditAnywhere, Category = "Tuning")
	bool HasDurability = false;
	
	UPROPERTY(EditAnywhere, Category = "Tuning")
	float Health = 20.f;
	
	UPROPERTY(EditAnywhere, Category = "Tuning")
	float DamageThreshold = 4.f;
	
	UPROPERTY(EditAnywhere, Category = "Tuning")
	float DamageCeiling = 10.f;
	
	// scoring
	UPROPERTY(EditAnywhere, Category = "Tuning")
	int32 CostPerHealth = 10; // current value is Health * CostPerHealth

public:	
	// what happens when this object enters the delivery zone
	void HandleStartZoneOverlap(int Zone);
	
	// what happens when this object leaves the delivery zone
	void HandleEndZoneOverlap();
};
