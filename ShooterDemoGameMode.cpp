// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterDemoGameMode.h"
#include "ShooterDemoHUD.h"
#include "ShooterDemoCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

AShooterDemoGameMode::AShooterDemoGameMode()
	: Super()
{
	HUDClass = AShooterDemoHUD::StaticClass();
}

void AShooterDemoGameMode::GameOver(bool PlayerDied)
{
	GetWorldTimerManager().ClearAllTimersForObject(this);

	if (!PlayerDied) {
		EndScore = Time;
		IsGameOver = true;

		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AShooterDemoGameMode::AddGameOverWidget, 0.5f, false);
	}
}

void AShooterDemoGameMode::BeginPlay()
{ 
	Super::BeginPlay();
}

void AShooterDemoGameMode::StartTimer() {

	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &AShooterDemoGameMode::IncrementTime, 0.1, true);
}

void AShooterDemoGameMode::IncrementTime() {
	Time += 0.1;

	if (TimeDec == 9) {
		TimeSec++;
		TimeDec = 0;
	}
	else
		TimeDec++;
}

void AShooterDemoGameMode::AddGameOverWidget() {
	if (IsValid(GameOverWidgetClass)) {
		GameOverWidget = CreateWidget(GetWorld(), GameOverWidgetClass);

		if (GameOverWidget != nullptr) {

			GameOverWidget->AddToViewport();

			UGameplayStatics::SetGamePaused(GetWorld(), true);
			APlayerController* PlayerCtrl = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerCtrl, GameOverWidget);
			PlayerCtrl->SetShowMouseCursor(true);
		}		
	}
}