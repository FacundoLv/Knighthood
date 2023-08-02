// Copyright Epic Games, Inc. All Rights Reserved.

#include "KnighthoodGameMode.h"
#include "KnighthoodCharacter.h"
#include "UObject/ConstructorHelpers.h"

AKnighthoodGameMode::AKnighthoodGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/MyBlueprints/BP_KnighthoodCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
