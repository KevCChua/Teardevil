// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructableObject.h"
#include "TeardevilCharacter.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ADestructableObject::ADestructableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructableComponent"));
	DestructibleComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ADestructableObject::BeginPlay()
{
	Super::BeginPlay();
	//OnActorHit.AddDynamic(this, &ADestructableObject::BeginOverlapDavid);
	DestructibleComponent->OnComponentBeginOverlap.AddDynamic(this, &ADestructableObject::OnBeginOverLap);
	//DestructibleComponent->OnComponentBeginOverlap.AddDynamic(this, &ADestructableObject::OnBeginOverLapTest);
	//RootComponent.BeginO
	//OnActorBeginOverlap.AddDynamic(this, &ADestructableObject::OnBeginOverLap);
	//OnActorHit.AddDynamic(this, &ADestructableObject::OnHit);
}

// Called every frame
void ADestructableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADestructableObject::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	//OtherActor->GetVelocity().ForwardVector;
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Hit Actor: %s"), *OtherActor->GetName()));
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Hit Velocity: %s"), *OtherActor->GetVelocity().ToString()));
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Hit Magnitude: %f"), OtherActor->GetVelocity().Size()));
	ATeardevilCharacter* Player = Cast<ATeardevilCharacter>(OtherActor);
	if(Player)
	{
		GEngine->AddOnScreenDebugMessage(20, 2.f, FColor::Green, FString::Printf(TEXT("Player Hit Magnitude: %f"), Player->GetCharacterMovement()->Velocity.Size()));
		GEngine->AddOnScreenDebugMessage(21, 2.f, FColor::Green, FString::Printf(TEXT("Player Hit Velocity: %s"), *Player->GetCharacterMovement()->Velocity.ToString()));
		Destroy();
	}
	else if(OtherActor->GetVelocity().Size() >= 100.0f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Hit Magnitude: %f"), OtherActor->GetVelocity().Size()));
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Hit Velocity: %s"), *OtherActor->GetVelocity().ToString()));
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Hit Actor: %s"), *OtherActor->GetName()));
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Break!")));
		Destroy();
	}
}

void ADestructableObject::OnBeginOverLap(class UPrimitiveComponent* OverlapComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	/*ATeardevilCharacter* Player = Cast<ATeardevilCharacter>(OtherActor);
	if(Player)
	{
		GEngine->AddOnScreenDebugMessage(20, 2.f, FColor::Green, FString::Printf(TEXT("Overlap Player Hit Magnitude: %f"), Player->GetCharacterMovement()->Velocity.Size()));
		GEngine->AddOnScreenDebugMessage(21, 2.f, FColor::Green, FString::Printf(TEXT("Overlap Player Hit Velocity: %s"), *Player->GetCharacterMovement()->Velocity.ToString()));
		//DestructibleComponent->
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(20, 2.f, FColor::Green, FString::Printf(TEXT("Overlap Other Object Hit Magnitude: %f"), OtherActor->GetVelocity().Size()));
		GEngine->AddOnScreenDebugMessage(21, 2.f, FColor::Green, FString::Printf(TEXT("Overlap Other Object Hit Velocity: %s"), *OtherActor->GetVelocity().ToString()));
	}*/
	if(OtherActor->GetVelocity().Size() >= BreakMagnitude || GetVelocity().Size() >= BreakMagnitude)
	{
		DestructibleComponent->ApplyRadiusDamage(BaseDamage, OtherActor->GetActorLocation(), DamageRadius, ImpulseStrength, true);
		OverlapComponent->SetCollisionProfileName("IgnoreOnlyPawn");
		//OverlapComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//ApplyRadiusDamage(float BaseDamage, const FVector& HurtOrigin, float DamageRadius, float ImpulseStrength, bool bFullDamage)
	}
}


