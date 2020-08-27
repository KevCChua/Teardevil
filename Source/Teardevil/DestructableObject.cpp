// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructableObject.h"
#include "TeardevilCharacter.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DestructibleComponent.h"
#include "TeardevilGameMode.h"

// Sets default values
ADestructableObject::ADestructableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructableComponent"));
	DestructibleComponent->SetupAttachment(RootComponent);
	DestructibleComponent->LargeChunkThreshold = 500.0f;
	BaseDamage = FLT_MAX;

	Score = 100.0f;
	Name = "Breakable Object";
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
	if(bCanThrowableBreak)
		DestructibleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
		//DestructibleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Overlap);
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
	if(this->Tags.Contains("Destroyed"))
		return;
	////////////////////////// TEST /////////////////////////////////////
	ATeardevilCharacter* Player = Cast<ATeardevilCharacter>(OtherActor);
	if (bCanPlayerBreak && Player)
	{
		if(Player->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
		{
			DestructibleComponent->ApplyRadiusDamage(BaseDamage, OtherActor->GetActorLocation(), DamageRadius, ImpulseStrength, true);
			GEngine->AddOnScreenDebugMessage(25, 2.f, FColor::Green, FString::Printf(TEXT("PUCNCHING BREAK")));
			((ATeardevilGameMode*)GetWorld()->GetAuthGameMode())->ObjectDestroyed(Score, Name);
			this->Tags.Add("Destroyed");
			return;
		}
		GEngine->AddOnScreenDebugMessage(21, 2.f, FColor::Green, FString::Printf(TEXT("Im A Player")));
	}
	else if(bCanThrowableBreak && OtherActor->Tags.Contains("Throwable"))
	{
		GEngine->AddOnScreenDebugMessage(21, 2.f, FColor::Green, FString::Printf(TEXT("Im A Throwable ")));
	}
	else if(bCanEnemyBreak && OtherActor->Tags.Contains("Dead"))
	{
		DestructibleComponent->ApplyRadiusDamage(BaseDamage, OtherComp->GetComponentLocation(), DamageRadius, ImpulseStrength, true);
		GEngine->AddOnScreenDebugMessage(21, 2.f, FColor::Green, FString::Printf(TEXT("Im A Ragdoll")));
		return;
	}
	else if(bCanBulletBreak && OtherActor->Tags.Contains("Bullet"))
	{
		GEngine->AddOnScreenDebugMessage(21, 2.f, FColor::Green, FString::Printf(TEXT("Im A Bullet")));
	}
	else
		return;

	/////////////////////////////////////////////////////////////////////
	
	if(OtherActor->GetVelocity().Size() >= BreakMagnitude || GetVelocity().Size() >= BreakMagnitude || OtherActor->Tags.Contains("Large"))
	{
		DestructibleComponent->ApplyRadiusDamage(BaseDamage, OtherActor->GetActorLocation(), DamageRadius, ImpulseStrength, true);
		GEngine->AddOnScreenDebugMessage(22, 2.f, FColor::Green, FString::Printf(TEXT("Yup I Broke")));
		((ATeardevilGameMode*)GetWorld()->GetAuthGameMode())->ObjectDestroyed(Score, Name);
		this->Tags.Add("Destroyed");
		//OverlapComponent->SetCollisionProfileName("IgnoreOnlyPawn");
		//OverlapComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//ApplyRadiusDamage(float BaseDamage, const FVector& HurtOrigin, float DamageRadius, float ImpulseStrength, bool bFullDamage)
	}
}


