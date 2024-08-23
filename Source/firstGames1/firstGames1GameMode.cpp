// Copyright Epic Games, Inc. All Rights Reserved.

#include "firstGames1GameMode.h"
#include "firstGames1Character.h"
#include "UObject/ConstructorHelpers.h"

AfirstGames1GameMode::AfirstGames1GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
