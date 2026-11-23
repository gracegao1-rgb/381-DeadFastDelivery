// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableKey.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class DEADFASTDELIVERY_API AInteractableKey : public AActor
{
	GENERATED_BODY()

public:
	AInteractableKey();

	/** Spin and bob, so the player can spot it from across the room. */
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
					   bool bFromSweep, const FHitResult& Sweep);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> PickupRange;

	// how fast the key spins
	UPROPERTY(EditAnywhere, Category = "Tuning", meta = (Units = "deg/s"))
	float SpinRate = 90.f;
	
	// Store the starting location of the actor
	FVector InitialLocation;
	
	// how high the actor bobs
	UPROPERTY(EditAnywhere, Category = "Tuning", meta = (Units = "cm"))
	float BobAmplitude = 50.0f;

	// How fast the actor bobs
	UPROPERTY(EditAnywhere, Category = "Tuning", meta = (Units = "cm/s"))
	float BobSpeed = 2.0f;
};