// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "TeardevilCharacter.generated.h"

UCLASS(config=Game)
class ATeardevilCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class USpringArmComponent* CameraBoom;

	/** Follow camera */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class UCameraComponent* FollowCamera;
public:
	ATeardevilCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	// Called for Right Stick Forward/Backward Input
	void RightStickForward(float Value);
	
	// Called for Right Stick Right/Left Input
	void RightStickRight(float Value);

	// Logic for Punch
	void Punch(float X, float Y, float DeltaTime);
	
	// Called When Key Pressed
	void DodgePressed();

	// Called When Key Released
	void DodgeReleased();

	// Called When Moving for Dodge
	void DodgeMovement(float DeltaTime);
	
	// Called When Dodging
	void DodgeTimer();
	
	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	//FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	//FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USphereComponent* LeftHandCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USphereComponent* RightHandCollision;
	
	// Global Variables
	UPROPERTY(BlueprintReadOnly, Category = MyVariables)
		float PunchAngle;
	UPROPERTY(BlueprintReadOnly, Category = MyVariables)
        float DodgeAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		float DodgeDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
        float DodgeAnimationSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		float DodgeDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		float DodgeSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		float DodgeStopInterpMargin;
	
	float RightStickX;
	float RightStickY;

	FVector CurrentLocation;
	FVector DodgeLocation;

	UPROPERTY(BlueprintReadOnly, Category = MyVariables)
		bool bIsPunching;
	UPROPERTY(BlueprintReadOnly, Category = MyVariables)
		bool bIsDodging;
	UPROPERTY(BlueprintReadWrite, Category = MyVariables)
    	bool bIsLeftPunching;
    UPROPERTY(BlueprintReadWrite, Category = MyVariables)
    	bool bIsRightPunching;
	
	bool bDodgeKeyHeld;
	bool bDodgeLoop;
	bool bSetActorX;
	bool bSetActorY;

	FTimerHandle DodgeTimerHandle;
};

