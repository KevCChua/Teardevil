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

	UPROPERTY(Category = "Global", BlueprintReadWrite, EditAnywhere)
	int32 MaxEnemies = 30;

	UPROPERTY(Category = "Global", BlueprintReadWrite, EditAnywhere)
	int32 CurrentEnemies = 0;

	UPROPERTY(Category = "Score", BlueprintReadWrite, EditAnywhere)
		float Timer = 0;
	UPROPERTY(Category = "Score", BlueprintReadWrite, EditAnywhere)
		float Destructable = 0;
	UPROPERTY(Category = "Score", BlueprintReadWrite, EditAnywhere)
		float Enemies = 0;
	UPROPERTY(Category = "Score", BlueprintReadWrite, EditAnywhere)
		float Score = 0;
	UPROPERTY(Category = "Multiplier", BlueprintReadWrite, EditAnywhere)
		float MULTI_TIMER_MAX = 5.0f;
	UPROPERTY(Category = "Multiplier", BlueprintReadWrite, EditAnywhere)
		float Multiplier = 1.0f;
	UPROPERTY(Category = "Multiplier", BlueprintReadWrite, EditAnywhere)
		float timerMultiplier = 0.f;

	UFUNCTION(BlueprintCallable)
		void RefreshMultiTimer()
	{
		timerMultiplier = MULTI_TIMER_MAX;
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
		Multiplier += 0.1;
	}

	void AddToScore(float _Score)
	{
		Score += _Score * Multiplier;
	}
};



