// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterDemoHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"

AShooterDemoHUD::AShooterDemoHUD()
{}

void AShooterDemoHUD::DrawHUD()
{
	Super::DrawHUD();

	// Find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// Offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X - 8.0f),
										   (Center.Y - 8.0f));

	// Draw the crosshair
	if (CrosshairTex) {
		FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
		TileItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(TileItem);
	}
}
