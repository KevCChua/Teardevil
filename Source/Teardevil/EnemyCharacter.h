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
	void Damaged(int Value, FVector Location);
	void DestroyEnemy(FVector Location);
	UFUNCTION(BlueprintImplementableEvent)
	void StopAIBehaviour();
	
	
	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		int Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		int Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
		float ImpactForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyVariables)
        float ImpactRadius;
	
};
