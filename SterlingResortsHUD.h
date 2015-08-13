// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "SterlingResortsHUD.generated.h"

UCLASS()
class ASterlingResortsHUD : public AHUD
{
	GENERATED_BODY()

public:
	ASterlingResortsHUD(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	bool bShowLogo;

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* LoaderLogo;

};

