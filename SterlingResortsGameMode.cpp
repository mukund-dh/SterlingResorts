// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SterlingResorts.h"
#include "SterlingResortsGameMode.h"
#include "SterlingResortsHUD.h"
#include "SterlingGameState.h"
#include "SterlingResortsCharacter.h"

ASterlingResortsGameMode::ASterlingResortsGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// set default game state class to our Blueprinted game state
	//GameStateClass = ASterlingGameState::StaticClass();

	// use our custom HUD class
	HUDClass = ASterlingResortsHUD::StaticClass();
}
