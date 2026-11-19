// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "HandsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADFASTDELIVERY_API UHandsComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UHandsComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	FVector LeftHandRelativeOffset = FVector(0.f, 0.f, 0.f);
	FVector RightHandRelativeOffset = FVector(0.f,0.f, 0.f);

// protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Hands")
	TObjectPtr<USphereComponent> LeftHand;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Hands")
	TObjectPtr<USphereComponent> RightHand;
	
	// used for rotation
	// UPROPERTY() USceneComponent* RightPivot;
	// UPROPERTY() USceneComponent* LeftPivot;

	// HandsComponent.h
	FRotator LeftHandRot = FRotator::ZeroRotator;
	FRotator RightHandRot = FRotator::ZeroRotator;
	FVector LeftPivotLoc = FVector(0, -20, 0); // pivot in view space, relative to pawn view location
	FVector RightPivotLoc = FVector(0, 20, 0);
	FVector HandOffset = FVector(150, 0, 0); // rest offset from the pivot

	void SetHandRotation(bool bRight, const FRotator& Rot);
	
	UPROPERTY(EditAnywhere, Category="Hands|Physics") float HandMassKg = 50.f;
	UPROPERTY(EditAnywhere, Category="Hands|Physics") float HandStiffness = 400.f;
	UPROPERTY(EditAnywhere, Category="Hands|Physics") float HandDamping = 40.f;
	UPROPERTY(EditAnywhere, Category="Hands|Physics") float HandMaxAccel = 20000.f;
	UPROPERTY(EditAnywhere, Category="Hands|Physics") float MaxHandLag = 100.f;
	UPROPERTY(EditAnywhere, Category="Hands") float MaxHandStep = 12.f;

private:
	USphereComponent* CreateHand(const FName Name);

	FVector LeftHandRelativeLocation = FVector(150.f, 40.f, -10.f);
	FVector RightHandRelativeLocation = FVector(150.f,-40.f, -10.f);
};
