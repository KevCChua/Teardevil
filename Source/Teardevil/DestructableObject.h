// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DestructibleComponent.h"
#include "DestructableObject.generated.h"

UCLASS()
class TEARDEVIL_API ADestructableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestructableObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Functions
	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
		void OnFracture(const FVector& HitPoint, const FVector& HitDirection);
	UFUNCTION()
		void OnBeginOverLap(class UPrimitiveComponent* OverlapComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
		void OnBeginOverLapTest(class UPrimitiveComponent* OverlapComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	void CollisionTimer();
	void DestroyTimer();
	// Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=MyDestructable)
		UDestructibleComponent* DestructibleComponent;
	
	// Variables
	float BaseDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MyProperties)
		float DamageRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MyProperties)
		float ImpulseStrength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MyProperties)
        float BreakMagnitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MyProperties)
		float Score;

	UPROPERTY(EditDefaultsOnly, Category=MyProperties)
		FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MyProperties)
		bool bCanPlayerBreak;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MyProperties)
		bool bCanEnemyBreak;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MyProperties)
		bool bCanThrowableBreak;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MyProperties)
		bool bCanBulletBreak;

	FTimerHandle CollisionTimerHandle;
	FTimerHandle DestroyTimerHandle;
};