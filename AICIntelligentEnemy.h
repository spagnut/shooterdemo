// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICEnemy.h"
#include "AICIntelligentEnemy.generated.h"

// AI Controller, controls intelligent enemies with perception
UCLASS()
class SHOOTERDEMO_API AAICIntelligentEnemy : public AAICEnemy
{
	GENERATED_BODY()

public:
	AAICIntelligentEnemy();

	UFUNCTION(BlueprintCallable)
	void OnPawnDetected(const TArray<AActor*>& DetectedPawns);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
	class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
	class UAISenseConfig_Hearing* HearingConfig;
};
