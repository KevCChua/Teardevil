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
};



