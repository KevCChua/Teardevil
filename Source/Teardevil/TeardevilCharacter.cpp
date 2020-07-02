// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TeardevilCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// ATeardevilCharacter

ATeardevilCharacter::ATeardevilCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	//// Create a camera boom (pulls in towards the player if there is a collision)
	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	//CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	//// Create a follow camera
	//FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	//FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create Left Hand Collider
	LeftHandCollision = CreateDefaultSubobject<USphereComponent>(TEXT("LeftHandCollision"));
	LeftHandCollision->SetupAttachment(this->GetMesh());
	
	// Create Right Hand Collider
	RightHandCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RightHandCollision"));
	RightHandCollision->SetupAttachment(this->GetMesh());

	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	// Default Values
	bSetActorX = true;
	bSetActorY = true;
	bIsLeftPunching = false;
	bIsRightPunching = false;
}

void ATeardevilCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Set Current Location To Variable
	CurrentLocation = GetActorLocation();
	// Call Function
	Punch(RightStickX, RightStickY, DeltaTime);
	// Check If Dodging
	if (bIsDodging)
	{
		// Call Function
		DodgeMovement(DeltaTime);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATeardevilCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATeardevilCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATeardevilCharacter::MoveRight);

	PlayerInputComponent->BindAxis("RightStickForward", this, &ATeardevilCharacter::RightStickForward);
	PlayerInputComponent->BindAxis("RightStickRight", this, &ATeardevilCharacter::RightStickRight);

	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ATeardevilCharacter::DodgePressed);
	PlayerInputComponent->BindAction("Dodge", IE_Released, this, &ATeardevilCharacter::DodgeReleased);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATeardevilCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATeardevilCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATeardevilCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATeardevilCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ATeardevilCharacter::OnResetVR);
}


void ATeardevilCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ATeardevilCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ATeardevilCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ATeardevilCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	
}

void ATeardevilCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATeardevilCharacter::MoveForward(float Value)
{
	// Checks If Controlled, Has Value, And If In Dodge
	if ((Controller != NULL) && (Value != 0.0f) && !bIsDodging)
	{
		// Set Rotation For Movement (Constant Since Camera Doesnt Rotate)
		const FRotator YawRotation(0, -90, 0);
		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ATeardevilCharacter::MoveRight(float Value)
{
	// Checks If Controlled, Has Value, And If In Dodge
	if ( (Controller != NULL) && (Value != 0.0f) && !bIsDodging)
	{
		// Set Rotation For Movement (Constant Since Camera Doesnt Rotate)
		const FRotator YawRotation(0, -90, 0);
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);		
	}
}

void ATeardevilCharacter::RightStickForward(float Value)
{
	// Set Value
	RightStickY = Value;
}

void ATeardevilCharacter::RightStickRight(float Value)
{
	// Set Value
	RightStickX = Value;
}

void ATeardevilCharacter::Punch(float X, float Y, float DeltaTime)
{
	// Get Angle Of Right Stick
	PunchAngle = FMath::RadiansToDegrees(FMath::Atan2(Y, X));
	// Check if Right Stick Pressed
	if (X != 0.0f || Y != 0.0f)
	{
		// Check If Dodge Key Is Held Down
		if(!bDodgeKeyHeld)
		{
			// Set Punching Variable
			
			// Rotate Character to Direction Pressed
			SetActorRotation(FMath::Lerp(GetActorRotation(), FRotator(0.0f, PunchAngle, 0.0f), 1 - FMath::Pow(FMath::Pow(0.7, 1 / DeltaTime), DeltaTime)));
			if(!bIsHolding)
			{
				bIsPunching = true;
				//SetActorRotation(FRotator(0.0f, PunchAngle, 0.0f));
				// Create Array to Hold Overlapping Actors
				TArray<AActor*> CollectedActors;
				// If Punching With Left Hand
				if (bIsLeftPunching)
				{
					// Get All Actors That Overlap Sphere
					LeftHandCollision->GetOverlappingActors(CollectedActors);
					// Iterate Through All Actors In Array
					for (int i = 0; i < CollectedActors.Num(); i++)
					{
						// Check If Actor is Self
						if (CollectedActors[i] != this)
						{
							// Debug
							GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Left Punched Actor Name: %s"), *CollectedActors[i]->GetName()));
							// Empty Array
							CollectedActors.Empty();
							// Stop Punching With Hand
							bIsLeftPunching = false;
							break;
						}
					}
				}
				// Else If Punching With Right Hand
				else if (bIsRightPunching)
				{
					// Get All Actors That Overlap Sphere
					RightHandCollision->GetOverlappingActors(CollectedActors);
					// Iterate Through All Actors In Array
					for (int i = 0; i < CollectedActors.Num(); i++)
					{
						// Check If Actor is Self
						if (CollectedActors[i] != this)
						{
							// Debug 
							GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Right Punched Actor Name: %s"), *CollectedActors[i]->GetName()));
							// Empty Array
							CollectedActors.Empty();
							// Stop Punching With Hand
							bIsRightPunching = false;
							break;
						}
					}
				}
			}
		}
		else
		{
			// Call Dodge Pressed Until Key Not Held Down
			DodgePressed();
		}
	}
	else
		// Set Punching Variable
		bIsPunching = false;
}

void ATeardevilCharacter::DodgePressed()
{
	// Set Key Held Variable to True
	bDodgeKeyHeld = true;
	// Check If Right Stick Has Any Values
	if (RightStickX != 0.0f || RightStickY != 0.0f)
	{
		// Since DodgePressed Is Called Every Frame Key is Held, Check If Executed Before 
		if (!bDodgeLoop)
		{
			// Set So Only Executes Once
			bDodgeLoop = true;
			// Set To True to Call Movement 
			bIsDodging = true;
			// Get Both Characters and Right Sticks Current Angle 
			const float PlayerRotation = GetActorRotation().Yaw;
			const float RightStickAngle = FMath::RadiansToDegrees(FMath::Atan2(RightStickY, RightStickX));
			// Get Normalized Directional Vector From Rotation
			const FVector DodgeVector = FRotator(0.0f, RightStickAngle, 0.0f).Vector();
			// Set Variable For Use In Animation Blueprints
			DodgeAngle = FMath::RadiansToDegrees(FMath::Atan2(RightStickY, RightStickX));
			DodgeAngleAnim = PlayerRotation - RightStickAngle;
			// Set Location To Go To
			DodgeLocation = GetActorLocation() + DodgeVector * DodgeDistance;


			//////////////////////////////////////////
			FRotator newDirection = FRotator(0.0f, DodgeAngle, 0.0f);
			FVector setVelocity = newDirection.Vector();
			if (GetCharacterMovement()->IsFalling())
			{
				DodgeVelocity = setVelocity * AirDodgeVelocityModifier;
				bDodgeInAir = true;
			}

			else
				DodgeVelocity = setVelocity * DodgeVelocityModifier;
			
			//DodgeVelocity = setVelocity;
			//////////////////////////////////////////
			
			// Set Timer
			GetWorld()->GetTimerManager().SetTimer(DodgeTimerHandle, this, &ATeardevilCharacter::DodgeTimer, DodgeDelay, false);
		}
	}
}

void ATeardevilCharacter::DodgeReleased()
{
	// Set Key Held Variable to False
	bDodgeKeyHeld = false;
}


void ATeardevilCharacter::DodgeMovement(float DeltaTime)
{
	if (bDodgeInAir)
	{
		GetCharacterMovement()->Velocity.X = FMath::FInterpTo(DodgeVelocity.X, 0.0f, DeltaTime, AirDodgeSpeed);
		GetCharacterMovement()->Velocity.Y = FMath::FInterpTo(DodgeVelocity.Y, 0.0f, DeltaTime, AirDodgeSpeed);
		GetCharacterMovement()->Velocity.Z -= 9.8;
	}
	else
	{
		GetCharacterMovement()->Velocity.X = FMath::FInterpTo(DodgeVelocity.X, 0.0f, DeltaTime, DodgeSpeed);
		GetCharacterMovement()->Velocity.Y = FMath::FInterpTo(DodgeVelocity.Y, 0.0f, DeltaTime, DodgeSpeed);
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Velocity X: %f"), GetCharacterMovement()->Velocity.X));
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Velocity Y: %f"), GetCharacterMovement()->Velocity.Y));
	DodgeVelocity.X = GetCharacterMovement()->Velocity.X;
	DodgeVelocity.Y = GetCharacterMovement()->Velocity.Y;
	
	float hypotenuse = FMath::Sqrt(FMath::Square(DodgeVelocity.X) + FMath::Square(DodgeVelocity.Y));
	if (hypotenuse <= 200.0f)
	{
		bIsDodging = false;
		bDodgeInAir = false;
	}
		
}

// void ATeardevilCharacter::DodgeMovement(float DeltaTime)
// {
// 	// Movement Separated By X and Y 
// 	// Check If Can Move In X Axis
// 	if (bSetActorX)
// 	{
// 		//Set X Only
// 		FVector X = DodgeLocation;
// 		X.Y = CurrentLocation.Y;
// 		X.Z = CurrentLocation.Z;
// 		// Check If Close To Destination (Within Margin)
// 		if (CurrentLocation.X  + DodgeStopInterpMargin >= X.X && CurrentLocation.X - DodgeStopInterpMargin <= X.X )
// 		{
// 			// Stop X Movement
// 			bSetActorX = false;
// 		}
// 		else
// 		{
// 			// Set Location And Check if Blocked With Sweep
// 			//bSetActorX = SetActorLocation(FMath::VInterpTo(CurrentLocation, X, DeltaTime, DodgeSpeed), true);
// 			
// 			// Reset Current Location In Case Y Also Moves
// 			CurrentLocation = GetActorLocation();
//
// 			//CharacterMovement->Velocity += 4*
// 		}
// 	}
// 	// Check If Can Move In Y Axis
// 	if (bSetActorY)
// 	{
// 		// Set Y Only
// 		FVector Y = DodgeLocation;
// 		Y.X = CurrentLocation.X;
// 		Y.Z = CurrentLocation.Z;
// 		// Check If Close To Destination (Within Margin)
// 		if (CurrentLocation.Y  + DodgeStopInterpMargin >= Y.Y && CurrentLocation.Y - DodgeStopInterpMargin <= Y.Y)
// 		{
// 			// Stop X Movement
// 			bSetActorY = false;
// 		}
// 		else
// 		{
// 			// Set Location And Check if Blocked With Sweep
// 			//bSetActorY = SetActorLocation(FMath::VInterpTo(CurrentLocation, Y, DeltaTime, DodgeSpeed), true);
// 		}
// 	}
//
// 	// Check If Both Axis Can Move (Stop Dodge if False)
// 	if(!bSetActorX && !bSetActorY)
// 			bIsDodging = false;	
//
// 	// Debug
// 	GEngine->AddOnScreenDebugMessage(10, 5.f, FColor::Green, FString::Printf(TEXT("Actor Location: %s"), *CurrentLocation.ToString()));
// 	GEngine->AddOnScreenDebugMessage(11, 5.f, FColor::Green, FString::Printf(TEXT("Dodge Location: %s"), *DodgeLocation.ToString()));
// 	GEngine->AddOnScreenDebugMessage(12, 5.f, FColor::Green, FString::Printf(TEXT("bSetActorX: %s"), ( bSetActorX ? TEXT("true") : TEXT("false") )));
// 	GEngine->AddOnScreenDebugMessage(13, 5.f, FColor::Green, FString::Printf(TEXT("bSetActorY: %s"), ( bSetActorY ? TEXT("true") : TEXT("false") )));
// }

void ATeardevilCharacter::DodgeTimer()
{
	// Reset Variables 
	bDodgeLoop = false;
	bIsDodging = false;
	bSetActorX = true;
	bSetActorY = true;
	// Clear Timer
	GetWorldTimerManager().ClearTimer(DodgeTimerHandle);
}
