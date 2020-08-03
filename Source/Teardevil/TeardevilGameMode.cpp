// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TeardevilGameMode.h"
#include "TeardevilCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATeardevilGameMode::ATeardevilGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Assets/Blueprints/Characters/TeardevilCharacterBP"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
