// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "HandsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADFASTDELIVERY_API UHandsComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	virtual void BeginPlay() override;
	
	void StartGrabbing();
	
	void StopGrabbing();
	
	UHandsComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	FVector LeftHandRelativeOffset = FVector(0.f, 0.f, 0.f);
	FVector RightHandRelativeOffset = FVector(0.f,0.f, 0.f);
	
	float GetLeftHandGrip() const {return LeftHandGrip;};
	float GetRightHandGrip() const {return RightHandGrip;};

// protected:
	
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Hands")
	TObjectPtr<USphereComponent> LeftHand;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Hands")
	TObjectPtr<USphereComponent> RightHand;
	
	UPROPERTY(Transient) 
	TObjectPtr<UPhysicsHandleComponent> LeftHandle;
	
	UPROPERTY(Transient) 
	TObjectPtr<UPhysicsHandleComponent> RightHandle;
	
	UPROPERTY(Transient) 
	FTransform LeftGrabOffset;
	
	UPROPERTY(Transient) 
	FTransform RightGrabOffset;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ArmMesh;

	// HandsComponent.h
	FRotator LeftHandRot = FRotator::ZeroRotator;
	FRotator RightHandRot = FRotator::ZeroRotator;
	FVector LeftPivotLoc = FVector(0, -20, 0); // pivot in view space, relative to pawn view location
	FVector RightPivotLoc = FVector(0, 20, 0);
	FVector HandOffset = FVector(200, 0, 0); // rest offset from the pivot

	void SetHandRotation(int HandOption, const FRotator& Rot);
	
	// arms
	UPROPERTY(VisibleAnywhere, Category = "Arm")
	TObjectPtr<UStaticMeshComponent> LeftArmSegment;

	UPROPERTY(VisibleAnywhere, Category = "Arm")
	TObjectPtr<UStaticMeshComponent> RightArmSegment;
	
	void UpdateArm(UStaticMeshComponent* Arm, USphereComponent* Hand, const FVector& ShoulderLocal, const FVector& Origin, const FQuat& ViewQuat);

	UPROPERTY(EditAnywhere, Category = "Arm")
	float Thickness = 20.f;
	
	UPROPERTY() TObjectPtr<UMaterialInterface> ArmBaseMaterial; // arm color

	UPROPERTY(EditAnywhere, Category = "Arm")
	FVector LeftShoulderLocal = FVector(20.f, -30.f, -35.f);

	UPROPERTY(EditAnywhere, Category = "Arm")
	FVector RightShoulderLocal = FVector(20.f, 30.f, -35.f);

private:
	USphereComponent* CreateHand(const FName Name, const FVector& HandRelativeLocation, TObjectPtr<UStaticMeshComponent>& OutArmSegment);
	
	UPhysicsHandleComponent* CreateHandle() const;
	
	void TryGrabbing(USphereComponent* Hand, UPhysicsHandleComponent* Handle, FTransform& OutGrabOffset);
	
	// Called in tick
	void UpdateGrip(UPhysicsHandleComponent* Handle, UPhysicsHandleComponent* OtherHandle, float& Grip, float DeltaTime);

	FVector LeftHandRelativeLocation = FVector(150.f, -40.f, -10.f);
	FVector RightHandRelativeLocation = FVector(150.f,40.f, -10.f);
	
	float LeftHandGrip = 1.f; // [0,1]
	float RightHandGrip = 1.f; // [0,1]
};
