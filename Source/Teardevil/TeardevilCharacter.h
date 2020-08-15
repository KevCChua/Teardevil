// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <string>
#include <openexr/Deploy/include/ImfArray.h>

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "TeardevilCharacter.generated.h"

USTRUCT(BlueprintType)
struct FAnimStruct
{
	GENERATED_USTRUCT_BODY()
	
    UPROPERTY(EditDefaultsOnly)
	UAnimSequenceBase* AnimAsset;

	UPROPERTY(EditDefaultsOnly)
	FName SlotName;
	
	UPROPERTY(EditDefaultsOnly)
	float AnimSpeed;

	UPROPERTY(EditDefaultsOnly)
	float StunDuration;
	
	UPROPERTY(EditDefaultsOnly)
	bool bIsFinishMove;
};

USTRUCT(BlueprintType)
struct FComboStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FAnimStruct> Combo;
};

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

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		void OnCapsuleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
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

	// Logic for Attacks
	void Attack();

	// Check Collision On Limbs
	void AttackCollision();

	// Move During Attack
	UFUNCTION(BlueprintCallable)
	void AttackMovement(float DeltaTime);

	// Logic for Animations
	void PlayAnimations(int Dir);

	// Check Closest Enemy
	void TargetEnemy();

	// Timer for Attack Animations
	void AttackTimer();

	// Timer for Frame Skip
	void FrameSkipTimer();
	
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

	// Functions
	UFUNCTION(BlueprintCallable)
	void PlayerDamaged(int Value);
	
	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USphereComponent* LeftHandCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USphereComponent* RightHandCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USphereComponent* RightFootCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USphereComponent* AttackCollider;
	
	// Global Variables
	UPROPERTY(BlueprintReadOnly, Category = MyVariables)
		float PunchAngle;
	UPROPERTY(BlueprintReadOnly, Category = MyVariables)
        float DodgeAngle;
	UPROPERTY(BlueprintReadOnly, Category = MyVariables)
        float DodgeAngleAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		float DodgeDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
        float DodgeAnimationSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		float DodgeDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		float DodgeSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
        float AirDodgeSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		float DodgeStopInterpMargin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
        float DodgeVelocityModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		float AirDodgeVelocityModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		float AnimPlayRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		float LeftTransitionPlayRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		float RightTransitionPlayRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		float BackTransitionPlayRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		float AttackVelocityModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		float AttackTravelSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		float ComboDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		float SnapToDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		float OffsetDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		float RotateSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		float FrameSkipDilation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		float FrameSkipDuration;

	float AttackDir;
	
	
	UPROPERTY(BlueprintReadOnly, Category = MyVariables)
		float RightStickX;
	UPROPERTY(BlueprintReadOnly, Category = MyVariables)
		float RightStickY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		float DeadZone;

	UPROPERTY(BlueprintReadOnly)
		TArray<int> DirectionArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		int Damage;

	int AttackCtr;
	int AnimationSequence;
	int LastAttackDir;
	int TransitionDir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		FVector AttackMoveLocation;
	
	FVector CurrentLocation;
	FVector DodgeLocation;
	FVector AttackVelocity;
	FVector AttackOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
        FVector DodgeVelocity;

	FRotator AttackRotation;

	UPROPERTY(BlueprintReadWrite, Category = MyVariables)
		bool bIsPunching;
	UPROPERTY(BlueprintReadWrite, Category = MyVariables)
		bool bIsDodging;
	UPROPERTY(BlueprintReadWrite, Category = MyVariables)
		bool bIsHolding;
	UPROPERTY(BlueprintReadWrite, Category = MyVariables)
    	bool bIsLeftPunching;
    UPROPERTY(BlueprintReadWrite, Category = MyVariables)
    	bool bIsRightPunching;
	UPROPERTY(BlueprintReadWrite, Category = MyVariables)
		bool bIsAttacking;
	UPROPERTY(BlueprintReadWrite, Category = MyVariables)
		bool bNextAttack;
	UPROPERTY(BlueprintReadWrite)
		bool bCollideDuringAnim;
	UPROPERTY(BlueprintReadWrite)
		bool bBufferedAttack;
	UPROPERTY(BlueprintReadWrite)
		bool bIsStunned;

	UPROPERTY(BlueprintReadOnly)
		bool bDodgeKeyHeld;
	bool bDodgeLoop;
	bool bSetActorX;
	bool bSetActorY;
	bool bDodgeInAir;
	bool bFinishMove;


	UPROPERTY(EditDefaultsOnly, Category = MyVariables)
        TSubclassOf<class AActor> Onomatopoeia;

	UPROPERTY(EditDefaultsOnly, Category = Animations)
	TArray<FComboStruct> AnimArray;
	
	//UPROPERTY(EditDefaultsOnly, Category = Animations)
	//	UAnimSequenceBase* AnimFirstAttack;
	//UPROPERTY(EditDefaultsOnly, Category = Animations)
	//	UAnimSequenceBase* AnimSecondAttack;
	//UPROPERTY(EditDefaultsOnly, Category = Animations)
	//	UAnimSequenceBase* AnimThirdAttack;
	UPROPERTY(EditDefaultsOnly, Category = Animations)
		UAnimSequenceBase* LeftTransitionAttack;
	UPROPERTY(EditDefaultsOnly, Category = Animations)
		UAnimSequenceBase* RightTransitionAttack;
	UPROPERTY(EditDefaultsOnly, Category = Animations)
		UAnimSequenceBase* BackTransitionAttack;
	UPROPERTY(EditDefaultsOnly, Category = Animations)
		UAnimSequenceBase* HitReaction;
	
	FTimerHandle DodgeTimerHandle;
	FTimerHandle FrameSkipHandle;

	UPROPERTY(BlueprintReadWrite)
		FTimerHandle AttackTimerHandle;
};

