// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SterlingResorts.h"
#include "SterlingResortsHUD.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "CanvasItem.h"

ASterlingResortsHUD::ASterlingResortsHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> LoaderLogoTexObj(TEXT("/Game/Assets/Textures/LOGO"));
	LoaderLogo = LoaderLogoTexObj.Object;
}


void ASterlingResortsHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X - (LoaderLogo->GetSurfaceWidth() * 0.5)),
										   (Center.Y - (LoaderLogo->GetSurfaceHeight() * 0.5f)));

	// draw the logo
	if (bShowLogo)
	{
		FCanvasTileItem TileItem(CrosshairDrawPosition, LoaderLogo->Resource, FLinearColor::White);
		TileItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(TileItem);
	}
}

