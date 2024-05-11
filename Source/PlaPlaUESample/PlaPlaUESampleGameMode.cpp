// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlaPlaUESampleGameMode.h"
#include "PlaPlaUESampleCharacter.h"
#include "UObject/ConstructorHelpers.h"

APlaPlaUESampleGameMode::APlaPlaUESampleGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
