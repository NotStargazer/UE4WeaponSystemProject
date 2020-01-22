// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "IDontUnderstandUE4GameMode.h"
#include "IDontUnderstandUE4HUD.h"
#include "IDontUnderstandUE4Character.h"
#include "UObject/ConstructorHelpers.h"

AIDontUnderstandUE4GameMode::AIDontUnderstandUE4GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AIDontUnderstandUE4HUD::StaticClass();
}
