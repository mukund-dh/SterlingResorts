// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "SterlingGameState.generated.h"

/**
 * 
 */
UCLASS()
class STERLINGRESORTS_API ASterlingGameState : public AGameState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	uint8 CurrentLevelID;
	
	
};
