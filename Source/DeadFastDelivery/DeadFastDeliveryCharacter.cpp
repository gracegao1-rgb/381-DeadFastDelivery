// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeadFastDeliveryCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DeadFastDelivery.h"

ADeadFastDeliveryCharacter::ADeadFastDeliveryCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
	
	// Attach Hands
	HandsComponent = CreateDefaultSubobject<UHandsComponent>(TEXT("HandsComponent"));
	HandsComponent->SetupAttachment(GetMesh(), FName("HandsSocket")); // attached to player's body
}

void ADeadFastDeliveryCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ADeadFastDeliveryCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ADeadFastDeliveryCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADeadFastDeliveryCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADeadFastDeliveryCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ADeadFastDeliveryCharacter::LookInput);
		
		// Rotating arms
		EnhancedInputComponent->BindAction(HandRotateAction, ETriggerEvent::Triggered, this, &ADeadFastDeliveryCharacter::HandRotate);
		EnhancedInputComponent->BindAction(HandScrollAction, ETriggerEvent::Triggered, this, &ADeadFastDeliveryCharacter::HandScroll);
		EnhancedInputComponent->BindAction(AltAction, ETriggerEvent::Started,this, &ADeadFastDeliveryCharacter::AltPressed);
		EnhancedInputComponent->BindAction(AltAction, ETriggerEvent::Completed, this, &ADeadFastDeliveryCharacter::AltReleased);
		
		// toggle/switch arm
		EnhancedInputComponent->BindAction(ToggleHandAction, ETriggerEvent::Started, this, &ADeadFastDeliveryCharacter::ToggleHand);
	}
	else
	{
		UE_LOG(LogDeadFastDelivery, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void ADeadFastDeliveryCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void ADeadFastDeliveryCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void ADeadFastDeliveryCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ADeadFastDeliveryCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void ADeadFastDeliveryCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void ADeadFastDeliveryCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

void ADeadFastDeliveryCharacter::ApplyHandRotation(float DeltaPitch, float DeltaYaw)
{
	if (!HandsComponent) return;

	FRotator& Rot = bControlRightHand ? RightHandRot : LeftHandRot;
	if (bControlRightHand)
	{
		Rot.Pitch = FMath::Clamp(Rot.Pitch + DeltaPitch,RightMinPitch,RightMaxPitch);
		Rot.Yaw = FMath::Clamp(Rot.Yaw + DeltaYaw,RightMinYaw,RightMaxYaw);
		Rot.Roll = 0.f;
	}
	else
	{
		Rot.Pitch = FMath::Clamp(Rot.Pitch + DeltaPitch,LeftMinPitch,LeftMaxPitch);
		Rot.Yaw = FMath::Clamp(Rot.Yaw + DeltaYaw,LeftMinYaw,LeftMaxYaw);
		Rot.Roll = 0.f;
	}

	HandsComponent->SetHandRotation(bControlRightHand, Rot);
}

void ADeadFastDeliveryCharacter::HandRotate(const FInputActionValue& Value)
{
	const FVector2D In = Value.Get<FVector2D>();
	const float Dt = GetWorld()->GetDeltaSeconds();
	ApplyHandRotation(In.Y * RotSpeed * Dt, In.X * RotSpeed * Dt);
}

void ADeadFastDeliveryCharacter::HandScroll(const FInputActionValue& Value)
{
	const float Notch = Value.Get<float>() * ScrollStep; // no delta time

	if (bAltHeld)
	{
		ApplyHandRotation(0.f, Notch); // yaw (left/right)
	}
	else
	{
		ApplyHandRotation(Notch, 0.f); // pitch (up/down)
	}
}

void ADeadFastDeliveryCharacter::ToggleHand()
{
	bControlRightHand = !bControlRightHand;
}