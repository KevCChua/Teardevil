// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"



#include "TeardevilCharacter.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TeardevilGameMode.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Weapon Collider
	Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(this->GetMesh());
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

void AEnemyCharacter::Damaged(int Value, FVector ComponentLocation, FVector ActorLocation, float StunDuration)
{
	Health -= Value;
	FVector KnockBackDirection = FVector(GetActorLocation().X - ActorLocation.X, GetActorLocation().Y - ActorLocation.Y, 0.0f);
	KnockBackDirection.Normalize();
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Enemy Health: %d"), Health));
	if (Health <= 0) 
	{
		DestroyEnemy(ComponentLocation);
		((ATeardevilGameMode*)GetWorld()->GetAuthGameMode())->EnemyDefeated();
	}
	else
	{
		((ATeardevilGameMode*)GetWorld()->GetAuthGameMode())->RefreshMultiTimer();
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Stun Duration: %f"), StunDuration));
		Stun(StunAnimation->SequenceLength);
		// Play Animation
		this->GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(StunAnimation, "UpperBody", 0.25f, 0.25f, 1);
		KnockBackDestination = GetActorLocation() + KnockBackDirection * KnockBackDistance;
		KnockBackVelocity = KnockBackDirection * KnockBackVelocityModifier; 
	}
		
	
}

void AEnemyCharacter::DamagedMovement(float DeltaTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, FString::Printf(TEXT("Velocity: %s"), *GetCharacterMovement()->Velocity.ToString()));
	SetActorLocation(FMath::Lerp(GetActorLocation(),KnockBackDestination, 0.2), true);
	//SetActorLocation(FMath::VInterpTo(GetActorLocation(), KnockBackDestination, DeltaTime, KnockBackSpeed), true);

	//GetCharacterMovement()->Velocity.X = FMath::FInterpTo(KnockBackVelocity.X, 0.0f, DeltaTime, KnockBackTravelSpeed);
	//GetCharacterMovement()->Velocity.Y = FMath::FInterpTo(KnockBackVelocity.Y, 0.0f, DeltaTime, KnockBackTravelSpeed);
	//KnockBackVelocity = GetCharacterMovement()->Velocity;
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

void AEnemyCharacter::MeleeAttack()
{
	this->GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(MeleeAnimation, "UpperBody", 0.25f, 0.25f, 1);
}

void AEnemyCharacter::RangedAttack()
{
	this->GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(RangedAnimation, "UpperBody", 0.25f, 0.25f, 1);
}

void AEnemyCharacter::AttackCollision()
{
	TArray<AActor*> CollectedActors;
	Weapon->GetOverlappingActors(CollectedActors, TSubclassOf<ATeardevilCharacter>());

	for (int i = 0; i < CollectedActors.Num(); i++)
	{
		ATeardevilCharacter* Player = Cast<ATeardevilCharacter>(CollectedActors[i]);
		if(Player && bIsAttacking)
		{
			Player->PlayerDamaged(Damage);
			bIsAttacking = false;
			break;
		}
	}
}


