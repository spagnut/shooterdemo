// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShooterDemoGameMode.generated.h"

class AEnemy;

UCLASS(minimalapi)
class AShooterDemoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AShooterDemoGameMode();

	void GameOver(bool PlayerDied);

	UPROPERTY(BlueprintReadOnly)
	float Time = 0;
	UPROPERTY(BlueprintReadOnly)
	int TimeSec = 0;
	UPROPERTY(BlueprintReadOnly)
	int TimeDec = 0;

	UPROPERTY(BlueprintReadOnly)
	int EnemiesLeft = 7;

	UPROPERTY(BlueprintReadOnly)
	float EndScore = 0;

	UPROPERTY(BlueprintReadOnly)
	bool IsGameOver = false;

	UPROPERTY(BlueprintReadWrite)
	TArray<AEnemy*> AllEnemies;

	UFUNCTION(BlueprintCallable)
	void StartTimer();


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Class Types")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	UUserWidget* GameOverWidget;

private:
	void IncrementTime();

	void AddGameOverWidget();
};



