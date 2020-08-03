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
		void OnBeginOverLap(class UPrimitiveComponent* OverlapComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
		void OnBeginOverLapTest(class UPrimitiveComponent* OverlapComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	// Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=MyDestructable)
		UDestructibleComponent* DestructibleComponent;
	
	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MyProperties)
		float BaseDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MyProperties)
		float DamageRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MyProperties)
		float ImpulseStrength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MyProperties)
        float BreakMagnitude;
};
