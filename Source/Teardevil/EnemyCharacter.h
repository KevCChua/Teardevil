// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class TEARDEVIL_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Functions
	UFUNCTION(BlueprintCallable)
	void Damaged(int Value, FVector Location, FVector ActorLocation, float StunDuration);
	UFUNCTION(BlueprintCallable)
	void DamagedMovement(float DeltaTime);
	void DestroyEnemy(FVector Location);
	UFUNCTION(BlueprintImplementableEvent)
	void StopAIBehaviour();
	UFUNCTION(BlueprintImplementableEvent)
    void Stun(float Duration);
	UFUNCTION(BlueprintCallable)
    void MeleeAttack();
	UFUNCTION(BlueprintCallable)
    void AttackCollision();

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Weapon;
	
	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		int Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		int Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		float ImpactForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
        float ImpactRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		float KnockBackDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		float KnockBackVelocityModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		float KnockBackTravelSpeed;

	UPROPERTY(BlueprintReadWrite)
		bool bIsAttacking;
	
	UPROPERTY(EditAnywhere)
		TSubclassOf<UCameraShake> PunchShake;

	FVector KnockBackVelocity;
	FVector KnockBackDestination;

	UPROPERTY(EditDefaultsOnly, Category = Animations)
		UAnimSequenceBase* StunAnimation;
	UPROPERTY(EditDefaultsOnly, Category = Animations)
		UAnimSequenceBase* MeleeAnimation;
	
};
