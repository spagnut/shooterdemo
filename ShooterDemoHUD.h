// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterDemoHUD.generated.h"

// HUD class
UCLASS()
class AShooterDemoHUD : public AHUD
{
	GENERATED_BODY()

public:
	AShooterDemoHUD();

	// Primary draw call for the HUD
	virtual void DrawHUD() override;

	// Crosshair asset pointer 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* CrosshairTex;

};

