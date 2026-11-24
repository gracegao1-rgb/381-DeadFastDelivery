// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "InteractableLock.generated.h"

UCLASS()
class DEADFASTDELIVERY_API AInteractableLock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableLock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// FVector StartLocation;
	FVector EndLocation;
	bool bUnlocked = false;
	
	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
					   bool bFromSweep, const FHitResult& Sweep);

public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;
	
	void Unlock();
	
	void DisableMeshCollision();
	bool bCollisionDisabled = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
	TObjectPtr<UStaticMeshComponent> TargetMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
	TObjectPtr<UStaticMeshComponent> BlockingMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
	TObjectPtr<UStaticMeshComponent> SignMesh;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* HatchCollision;
	
	void RunAway();
};