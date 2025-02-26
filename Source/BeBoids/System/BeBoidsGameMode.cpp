// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeBoidsGameMode.h"
#include "UObject/ConstructorHelpers.h"

ABeBoidsGameMode::ABeBoidsGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
