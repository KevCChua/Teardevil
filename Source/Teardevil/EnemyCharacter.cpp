// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Health = 1;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::Damaged(int Value, FVector ComponentLocation, FVector ActorLocation)
{
	Health -= Value;
	FVector KnockBackDirection = FVector(GetActorLocation().X - ActorLocation.X, GetActorLocation().Y - ActorLocation.Y, 0.0f);
	KnockBackDirection.Normalize();
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Enemy Health: %d"), Health));
	if(Health <= 0)
		DestroyEnemy(ComponentLocation);
	else
		SetActorLocation(GetActorLocation() + KnockBackDirection * KnockBackDistance);
}

void AEnemyCharacter::DestroyEnemy(FVector Location)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Destroyed")));
	this->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	this->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->GetMesh()->SetSimulatePhysics(true);
	this->GetMesh()->AddRadialImpulse(Location, ImpactRadius, ImpactForce, ERadialImpulseFalloff::RIF_Constant, true);
	StopAIBehaviour();
	Tags.Add("Dead");
}

