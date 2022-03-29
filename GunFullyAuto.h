// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GunParent.h"
#include "GunFullyAuto.generated.h"

// Fully automatic gun class
UCLASS()
class SHOOTERDEMO_API AGunFullyAuto : public AGunParent
{
	GENERATED_BODY()

public:
	
	AGunFullyAuto();

	virtual void StartShooting() override;

	virtual void StopShooting() override;

	virtual void StartAlternateFire() override;

	virtual void StopAlternateFire() override;

	virtual void Tick(float DeltaTime) override;

private:

	bool CanShoot = true;

	FTimerHandle AutoFireTimerHandle;

	void ResetShooting();
};
