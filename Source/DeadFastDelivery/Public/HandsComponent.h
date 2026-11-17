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
	FVector RightHandRelativeOffset = FVector(0.f,  0.f, 0.f);

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Hands")
	TObjectPtr<USphereComponent> LeftHand;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Hands")
	TObjectPtr<USphereComponent> RightHand;

private:
	USphereComponent* CreateHand(const FName Name, const FVector& RelativeLocation);

	FVector LeftHandRelativeLocation = FVector(150.f, 40.f, -10.f);
	FVector RightHandRelativeLocation = FVector(150.f,  -40.f, -10.f);
};
