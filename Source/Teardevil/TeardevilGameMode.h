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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
		float Timer;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
		float Destructable;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
		float Enemies;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
		float Score;
};



