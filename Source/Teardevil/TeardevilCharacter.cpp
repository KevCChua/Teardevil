// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TeardevilCharacter.h"

#include <openexr/Deploy/include/ImfArray.h>


#include "EnemyCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

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

	// Create Right Foot Collider
	RightFootCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RightFootCollision"));
	RightFootCollision->SetupAttachment(this->GetMesh());

	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	// Default Values
	bSetActorX = true;
	bSetActorY = true;
	bIsLeftPunching = false;
	bIsRightPunching = false;

	bNextAttack = true;

	AttackOffset = FVector(0,0,0);
}

void ATeardevilCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Set Current Location To Variable
	CurrentLocation = GetActorLocation();
	// Call Function
	//Punch(RightStickX, RightStickY, DeltaTime);
	Attack();
	if(bIsAttacking)
		AttackCollision();
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


void ATeardevilCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATeardevilCharacter::OnCapsuleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//if(bIsAttacking)
	//{
		AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OtherActor);
		if(Enemy)
		{
			bCollideDuringAnim = true;
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString::Printf(TEXT("Enemy Collide: %s"), *OtherActor->GetName()));
		}
	//}
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString::Printf(TEXT("Enemy Hit: %s"), *OtherActor->GetName()));
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
	if ((Controller != NULL) && (Value != 0.0f) && !bIsDodging && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
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
	if ( (Controller != NULL) && (Value != 0.0f) && !bIsDodging && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
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
	//if(Value != 0.0f && !bDodgeKeyHeld && bIsHolding)
	//	bIsPunching = true;
}

void ATeardevilCharacter::RightStickRight(float Value)
{
	// Set Value
	RightStickX = Value;
	//if(Value != 0.0f && !bDodgeKeyHeld && bIsHolding)
	//	bIsPunching = true;
}

void ATeardevilCharacter::Punch(float X, float Y, float DeltaTime)
{
	// Get Angle Of Right Stick
	PunchAngle = FMath::RadiansToDegrees(FMath::Atan2(Y, X));
	// Check if Right Stick Pressed
	if (X != 0.0f || Y != 0.0f || bIsLeftPunching || bIsRightPunching)
	{
		// Check If Dodge Key Is Held Down
		if(!bDodgeKeyHeld)
		{
			if(!bIsHolding && !bIsDodging)
			{
				// Rotate Character to Direction Pressed
				SetActorRotation(FMath::Lerp(GetActorRotation(), FRotator(0.0f, X != 0 || Y != 0 ? PunchAngle : GetActorRotation().Yaw, 0.0f), 1 - FMath::Pow(FMath::Pow(0.7, 1 / DeltaTime), DeltaTime)));
				// Set Punching Variable
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
							AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(CollectedActors[i]);
							if(Enemy)
							{
								Enemy->Damaged(Damage, LeftHandCollision->GetComponentLocation(), GetActorLocation());
								GetWorld()->SpawnActor<AActor>(Onomatopoeia,Enemy->GetActorLocation(),FRotator(0, -90, 0));
								// Empty Array
								CollectedActors.Empty();							
								// Stop Punching With Hand
								bIsLeftPunching = false;
								break;
							}
						}
					}
					bIsPunching = false;
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
							AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(CollectedActors[i]);
							if(Enemy)
							{
								Enemy->Damaged(Damage, RightHandCollision->GetComponentLocation(), GetActorLocation());
								GetWorld()->SpawnActor<AActor>(Onomatopoeia,Enemy->GetActorLocation(),FRotator(0, -90, 0));
								// Empty Array
								CollectedActors.Empty();
								// Stop Punching With Hand
								bIsRightPunching = false;
								break;
							}
						}
					}
					bIsPunching = false;
				}
			}
		}
		else
		{
			// Call Dodge Pressed Until Key Not Held Down
			DodgePressed();
		}
	}
	//else
		// Set Punching Variable
		//bIsPunching = false;
}

void ATeardevilCharacter::Attack()
{
	// Get Angle Of Right Stick
	float AttackDirection = FMath::RadiansToDegrees(FMath::Atan2(RightStickY, RightStickX));
	//GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Green, FString::Printf(TEXT("Direction Pressed: %f"), AttackDirection));
	
	if(abs(RightStickX) > DeadZone || abs(RightStickY) > DeadZone)
	{
		if(!bDodgeKeyHeld)
		{
			if(!bIsHolding && !bIsDodging && bNextAttack)
			{
				//Do it
				// Up
				if(AttackDirection <= -67.5f && AttackDirection >= -112.5f)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Forward")));
					if(!DirectionArray.Contains(0))
						DirectionArray.Add(0);
					else
					{
						DirectionArray.Remove(0);
						DirectionArray.Add(0);
					}
				}
				// Forward-Right
				else if(AttackDirection <= -22.5f && AttackDirection >= -67.5f)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Forward-Right")));
					if(!DirectionArray.Contains(1))
						DirectionArray.Add(1);
					else
					{
						DirectionArray.Remove(1);
						DirectionArray.Add(1);
					}
				}
				// Right
				else if(AttackDirection <= 22.5f && AttackDirection >= -22.5f)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Right")));
					if(!DirectionArray.Contains(2))
						DirectionArray.Add(2);
					else
					{
						DirectionArray.Remove(2);
						DirectionArray.Add(2);
					}
				}
				// Back-Right
				else if(AttackDirection <= 67.5f && AttackDirection >= 22.5f)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Back-Right")));
					if(!DirectionArray.Contains(3))
						DirectionArray.Add(3);
					else
					{
						DirectionArray.Remove(3);
						DirectionArray.Add(3);
					}
				}
				// Back
				else if(AttackDirection <= 112.5f && AttackDirection >= 67.5f)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Back")));
					if(!DirectionArray.Contains(4))
						DirectionArray.Add(4);
					else
					{
						DirectionArray.Remove(4);
						DirectionArray.Add(4);
					}
				}
				// Back-Left
				else if(AttackDirection <= 157.5f && AttackDirection >= 112.5f)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Back-Left")));
					if(!DirectionArray.Contains(5))
						DirectionArray.Add(5);
					else
					{
						DirectionArray.Remove(5);
						DirectionArray.Add(5);
					}
				}
				// Left
				else if(AttackDirection <= -157.5f || AttackDirection >= 157.5f)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Left")));
					if(!DirectionArray.Contains(6))
						DirectionArray.Add(6);
					else
					{
						DirectionArray.Remove(6);
						DirectionArray.Add(6);
					}
				}
				// Forward-Left
				else if(AttackDirection <= -112.5f && AttackDirection >= -157.5f)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Forward-Left")));
					if(!DirectionArray.Contains(7))
						DirectionArray.Add(7);
					else
					{
						DirectionArray.Remove(7);
						DirectionArray.Add(7);
					}
				}
			}
		}
		else
		{
			DirectionArray.Empty();
			AttackCtr = 0;
			DodgePressed();
		}
	}
	else
	{
		//Reset
		int NumInArray = DirectionArray.Num();
		if(NumInArray >= 6)
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Full Circle")));
		/*else if(NumInArray >= 4)
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Half Circle")));
		else if(NumInArray >= 3)
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Quarter Circle")));*/
		else if(NumInArray >= 1)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Single Punch")));
			//if(!this->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
			PlayAnimations(DirectionArray.Top());
		}
		//else
		//	DirectionArray.Empty();

		/*if(DirectionArray.Num() != 0)
		{
			int LastIndex = DirectionArray.Top();
			switch (LastIndex)
			{
			case 0:
				SetActorRotation(FRotator(0.0f, -90.0f, 0.0f));
				break;
			case 1:
				SetActorRotation(FRotator(0.0f, -45.0f, 0.0f));
				break;
			case 2:
				SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
				break;
			case 3:
				SetActorRotation(FRotator(0.0f, 45.0f, 0.0f));
				break;
			case 4:
				SetActorRotation(FRotator(0.0f, 90.0f, 0.0f));
				break;
			case 5:
				SetActorRotation(FRotator(0.0f, 135.0f, 0.0f));
				break;
			case 6:
				SetActorRotation(FRotator(0.0f, 180.0f, 0.0f));
				break;
			case 7:
				SetActorRotation(FRotator(0.0f, -135.0f, 0.0f));
				break;
			
			default:
				break;
			}
		}*/
		DirectionArray.Empty();
	}
}

void ATeardevilCharacter::AttackCollision()
{
	TArray<AActor*> CollectedActors;
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Attack Counter: %d"), AttackCtr));
	USphereComponent* ActiveComponent = nullptr;
	if(TransitionDir == 0)
	{
		switch (AttackCtr)
		{
		case 0:
			RightFootCollision->GetOverlappingActors(CollectedActors);
			ActiveComponent = RightFootCollision;
			break;
		case 1:
			LeftHandCollision->GetOverlappingActors(CollectedActors);
			ActiveComponent = LeftHandCollision;
			break;
		case 2:
			RightHandCollision->GetOverlappingActors(CollectedActors);
			ActiveComponent = RightHandCollision;
			break;
		default:
			break;
		}
	}
	else
	{
		switch (TransitionDir)
		{
			case 1:
				RightHandCollision->GetOverlappingActors(CollectedActors);
				ActiveComponent = RightHandCollision;
				break;
			case 2:
				LeftHandCollision->GetOverlappingActors(CollectedActors);
				ActiveComponent = LeftHandCollision;
				break;
			case 3:
				RightFootCollision->GetOverlappingActors(CollectedActors);
				ActiveComponent = RightFootCollision;
				break;
			default:
				break;
		}
	}

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Active Component: %s"), *ActiveComponent->GetName()));
	
	// Iterate Through All Actors In Array
	for (int i = 0; i < CollectedActors.Num(); i++)
	{
		// Check If Actor is Self
		if (CollectedActors[i] != this)
		{
			// Debug
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("ActorHit: %s"), *CollectedActors[i]->GetName()));
			AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(CollectedActors[i]);
			if(Enemy)
			{
				Enemy->Damaged(Damage, ActiveComponent->GetComponentLocation(), GetActorLocation());
				GetWorld()->SpawnActor<AActor>(Onomatopoeia,Enemy->GetActorLocation(),FRotator(0, -90, 0));
				// Empty Array
				CollectedActors.Empty();
				bIsAttacking = false;
				break;
			}
		}
	}
}

void ATeardevilCharacter::AttackMovement(float DeltaTime)
{
	if(!bCollideDuringAnim)
	{
		GetCharacterMovement()->Velocity.X = FMath::FInterpTo(AttackVelocity.X, 0.0f, DeltaTime, AttackTravelSpeed);
		GetCharacterMovement()->Velocity.Y = FMath::FInterpTo(AttackVelocity.Y, 0.0f, DeltaTime, AttackTravelSpeed);
		AttackVelocity = GetCharacterMovement()->Velocity;
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("Velocity: %s"), *AttackVelocity.ToString()));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, FString::Printf(TEXT("Stop Moving")));
	}
	SetActorRotation(FMath::Lerp(GetActorRotation(), AttackRotation, DeltaTime * RotateSpeed));
}

void ATeardevilCharacter::PlayAnimations(int Dir)
{
	bNextAttack = false;
	//this->GetMesh()->GetAnimInstance()->StopAllMontages(0);
	if(AttackCtr == 0 || (Dir <= LastAttackDir + 1 && Dir >= LastAttackDir - 1) || (Dir == 7 && LastAttackDir == 0) || (Dir == 0 && LastAttackDir == 7))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Plus or Minus One")));
		switch(AttackCtr)
		{
		case 0:
			this->GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(AnimFirstAttack, "UpperBody", 0.25f, 0.25f, AnimPlayRate);
			// Set Timer
			GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &ATeardevilCharacter::AttackTimer, ComboDelay + (AnimFirstAttack->SequenceLength / AnimPlayRate), false);
			break;
		case 1:
			this->GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(AnimSecondAttack, "UpperBody", 0.25f, 0.25f, AnimPlayRate);
			// Set Timer
			GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &ATeardevilCharacter::AttackTimer, ComboDelay+ (AnimSecondAttack->SequenceLength / AnimPlayRate), false);
			break;
		case 2:
			this->GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(AnimThirdAttack, "UpperBody");
			// Clear Timer
			GetWorldTimerManager().ClearTimer(AttackTimerHandle);
			break;
		default:
			break;
		}
		if(AttackCtr >= 2)
			AttackCtr = 0;
		else
			AttackCtr++;
		
		TransitionDir = 0;
	}
	else if(Dir == LastAttackDir - 2 || (Dir == 7 && LastAttackDir == 1) || (Dir == 6 && LastAttackDir == 0))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Left Transition")));
		this->GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(LeftTransitionAttack, "UpperBody", 0.25f, 0.25f, LeftTransitionPlayRate);
		AttackCtr = 0;
		TransitionDir = 1;
		GetWorldTimerManager().ClearTimer(AttackTimerHandle);
	}
	else if (Dir == LastAttackDir + 2 || (Dir == 0 && LastAttackDir == 6) || (Dir == 1 && LastAttackDir == 7))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Right Transition")));
		this->GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(RightTransitionAttack, "UpperBody", 0.25f, 0.25f, RightTransitionPlayRate);
		AttackCtr = 0;
		TransitionDir = 2;
		GetWorldTimerManager().ClearTimer(AttackTimerHandle);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Back Transition")));
		this->GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(BackTransitionAttack, "UpperBody", 0.25f, 0.25f, BackTransitionPlayRate);
		AttackCtr = 0;
		TransitionDir = 3;
		GetWorldTimerManager().ClearTimer(AttackTimerHandle);
	}
	
	LastAttackDir = Dir;


	switch (Dir)
	{
		case 0:
			AttackOffset = FVector(0, -OffsetDistance, 0);
			AttackDir = -90;
			break;
		case 1:
			AttackOffset = FVector(OffsetDistance / 1.41421f,-OffsetDistance / 1.41421f,0);
			AttackDir = -45;
			break;
		case 2:
			AttackOffset = FVector(OffsetDistance, 0, 0);
			AttackDir = 0;
			break;
		case 3:
			AttackOffset = FVector(OffsetDistance / 1.41421f,OffsetDistance / 1.41421f,0);
			AttackDir = 45;
			break;
		case 4:
			AttackOffset = FVector(0, OffsetDistance, 0);
			AttackDir = 90;
			break;
		case 5:
			AttackOffset = FVector(-OffsetDistance / 1.41421f,OffsetDistance / 1.41421f,0);
			AttackDir = 135;
			break;
		case 6:
			AttackOffset = FVector(-OffsetDistance, 0, 0);
			AttackDir = 180;
			break;
		case 7:
			AttackOffset = FVector(-OffsetDistance / 1.41421f,-OffsetDistance / 1.41421f,0);
			AttackDir = -135;
			break;
		default:
			AttackOffset = FVector(0,0,0);
			break;
	}
	//FVector setVelocity = ;
	AttackVelocity = FRotator(0.0f, AttackDir, 0.0f).Vector() * AttackVelocityModifier;
	TargetEnemy();
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Attack Velocity: %s"), *AttackVelocity.ToString()));
}

void ATeardevilCharacter::TargetEnemy()
{
	TArray<AActor*> CollectedActors;
	AActor* ClosestActor = nullptr;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), CollectedActors);

	for (int i = 0; i < CollectedActors.Num(); i++)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Collected Actors: %s"), *CollectedActors[i]->GetName()));
		if(ClosestActor == nullptr && !CollectedActors[i]->Tags.Contains("Dead"))
			ClosestActor = CollectedActors[i];
		else
		{
			// Check If Distance is Greater
			if(!CollectedActors[i]->Tags.Contains("Dead") && ((GetActorLocation() + AttackOffset - ClosestActor->GetActorLocation()).Size() > (GetActorLocation() + AttackOffset - CollectedActors[i]->GetActorLocation()).Size() || ClosestActor == nullptr))
			{
				// Assign Closest Actor
				ClosestActor = CollectedActors[i];
			}
		}
	}
	if(ClosestActor != nullptr && (GetActorLocation() + AttackOffset - ClosestActor->GetActorLocation()).Size() <= SnapToDistance)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Closest Actor: %s"), *ClosestActor->GetName()));
		
		FVector Direction = FVector(ClosestActor->GetActorLocation().X - GetActorLocation().X, ClosestActor->GetActorLocation().Y - GetActorLocation().Y, 0.0f);
		Direction.Normalize();
		AttackVelocity = Direction * AttackVelocityModifier;
		//SetActorRotation(Direction.Rotation());
		AttackRotation = Direction.Rotation();
	}
	else
	{
		AttackVelocity = FRotator(0.0f, AttackDir, 0.0f).Vector() * AttackVelocityModifier;
		//SetActorRotation(FRotator(0.0f, AttackDir, 0.0f));
		AttackRotation = FRotator(0.0f, AttackDir, 0.0f);
	}
}

void ATeardevilCharacter::AttackTimer()
{
	AttackCtr = 0;
	TransitionDir = 0;
	bNextAttack = true;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Combo Ended")));
	// Clear Timer
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);
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

	GEngine->AddOnScreenDebugMessage(11, 2.f, FColor::Red, FString::Printf(TEXT("Velocity X: %f"), GetCharacterMovement()->Velocity.X));
	GEngine->AddOnScreenDebugMessage(12, 2.f, FColor::Green, FString::Printf(TEXT("Velocity Y: %f"), GetCharacterMovement()->Velocity.Y));
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
