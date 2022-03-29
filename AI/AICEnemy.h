// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AICEnemy.generated.h"

// AI Controller, controls Enemies

UCLASS()
class SHOOTERDEMO_API AAICEnemy : public AAIController
{
	GENERATED_BODY()
	
public:
	AAICEnemy();

	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(transient)
		class UBehaviorTreeComponent* BehaviorTreeUsed;
	UPROPERTY(transient)
		class UBlackboardComponent* BlackboardUsed;

	// Blackboard key Ids
	uint8 EnemyKeyId;
	uint8 TargetLocationKeyId;

	float DefaultWalkSpeed;
	float CombatWalkSpeed;

	//Functions
	void SetTarget(AController* DamageInstigator);

	void ClearTarget();

	UFUNCTION(BlueprintCallable)
	void Attack();

	
};
