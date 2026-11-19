// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HandsComponent.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "DeadFastDeliveryCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class ADeadFastDeliveryCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;
	
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHandsComponent* HandsComponent;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;
	
public:
	ADeadFastDeliveryCharacter();

protected:

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* GrabAction;

	UPROPERTY(EditAnywhere, Category="Input") UInputAction* PauseAction;
	
	virtual void TogglePause();
	
	// for arm rotation
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* HandRotateAction;
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* HandScrollAction;
	UPROPERTY(EditAnywhere, Category="Input") UInputAction* AltAction;

	UPROPERTY(EditAnywhere, Category="Hand") float RotSpeed = 90.f; // deg/sec for arrows
	UPROPERTY(EditAnywhere, Category="Hand") float ScrollStep = 5.f; // deg per wheel notch

	bool bAltHeld = false;

	void HandRotate(const FInputActionValue& Value);
	void HandScroll(const FInputActionValue& Value);
	void AltPressed() { bAltHeld = true; }
	void AltReleased() { bAltHeld = false; }
	
	FRotator RightHandRot = FRotator::ZeroRotator;
	FRotator LeftHandRot = FRotator::ZeroRotator;
	void ApplyHandRotation(float DeltaPitch, float DeltaYaw);
	
	UPROPERTY(EditAnywhere, Category="Hands|Limits") float RightMinPitch = -30.f;
	UPROPERTY(EditAnywhere, Category="Hands|Limits") float RightMaxPitch = 30.f;
	UPROPERTY(EditAnywhere, Category="Hands|Limits") float RightMinYaw = -50.f;
	UPROPERTY(EditAnywhere, Category="Hands|Limits") float RightMaxYaw = 50.f;
	
	UPROPERTY(EditAnywhere, Category="Hands|Limits") float LeftMinPitch = -30.f;
	UPROPERTY(EditAnywhere, Category="Hands|Limits") float LeftMaxPitch = 30.f;
	UPROPERTY(EditAnywhere, Category="Hands|Limits") float LeftMinYaw = -50.f;
	UPROPERTY(EditAnywhere, Category="Hands|Limits") float LeftMaxYaw = 50.f;
	
	// toggle/switch hand
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ToggleHandAction;
	
	// toggle drift
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ToggleDriftAction;

	/**
	 * 0 - Both hands (default)
	 * 1 - right hand
	 * 2 - left hand
	 */
	int HandOption = 0;

	void ToggleHand();
	
	void Tick(float DeltaTime);
	bool bShouldDriftDown = false;
	
	void ToggleDrift();

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

