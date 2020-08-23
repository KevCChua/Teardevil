// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TeardevilGameMode.generated.h"

UCLASS(minimalapi)
class ATeardevilGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATeardevilGameMode();
	// Enemy
	UPROPERTY(Category = "Global", BlueprintReadWrite, EditAnywhere)
	int32 MaxEnemies = 30;
	UPROPERTY(Category = "Global", BlueprintReadWrite, EditAnywhere)
	int32 CurrentEnemies = 0;
	//Score
	UPROPERTY(Category = "Score", BlueprintReadWrite, EditAnywhere)
		float Timer = 0;
	UPROPERTY(Category = "Score", BlueprintReadWrite, EditAnywhere)
		float Destructable = 0;
	UPROPERTY(Category = "Score", BlueprintReadWrite, EditAnywhere)
		float Enemies = 0;
	UPROPERTY(Category = "Score", BlueprintReadWrite, EditAnywhere)
		float Score = 0;
	//Multiplier
	UPROPERTY(Category = "Multiplier", BlueprintReadWrite, EditAnywhere)
		bool StartCombo = false;
	UPROPERTY(Category = "Multiplier", BlueprintReadWrite, EditAnywhere)
		float MultiTimerBuffer = 2.0f;
	UPROPERTY(Category = "Multiplier", BlueprintReadWrite, EditAnywhere)
		float MULTI_TIMER_MAX = 5.0f;
	UPROPERTY(Category = "Multiplier", BlueprintReadWrite, EditAnywhere)
		float Multiplier = 1.0f;
	UPROPERTY(Category = "Multiplier", BlueprintReadWrite, EditAnywhere)
		float timerMultiplier = 0.f;
	//Notoriety
	UPROPERTY(Category = "Notoriety", BlueprintReadWrite, EditAnywhere)
		float Notoriety = 0.f;
	UPROPERTY(Category = "Notoriety", BlueprintReadWrite, EditAnywhere)
		float NotorieryMax = 120.f;
	UPROPERTY(Category = "Notoriety", BlueprintReadWrite, EditAnywhere)
		float NotorieryDecreasePerSecond = 1.f;


	UFUNCTION(BlueprintCallable)
	void RefreshMultiTimer()
	{
		timerMultiplier = MULTI_TIMER_MAX + MultiTimerBuffer;
		StartCombo = true;
	}

	void EnemyDefeated(float _score) {
		Enemies += 1.f;
		AddToScore(_score);
		RefreshMultiTimer();
		Multiplier += 0.5;
	}

	UFUNCTION(BlueprintCallable)
	void ObjectDestroyed(float _score) {
		Destructable += 1.f;
		AddToScore(_score);
		RefreshMultiTimer();
		Multiplier += 0.25;
	}
	
	UFUNCTION(BlueprintCallable)
	void AddToScore(float _Score)
	{
		Score += _Score * Multiplier;
		AddToNotoriety(_Score / 100.0f);
	}

	UFUNCTION(BlueprintCallable)
	void AddToNotoriety(float _Notoriety)
	{
		Notoriety += _Notoriety * Multiplier;
	}

	UFUNCTION(BlueprintCallable)
	void MinusNotoriety(float _Notoriety)
	{
		Notoriety -= _Notoriety;
	}

};



