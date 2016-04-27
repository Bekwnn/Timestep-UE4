// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Timestep.h"
#include "TimestepGameMode.h"
#include "TimestepHUD.h"
#include "TimestepCharacter.h"

ATimestepGameMode::ATimestepGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ATimestepHUD::StaticClass();
}
