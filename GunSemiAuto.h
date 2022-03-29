// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GunParent.h"
#include "GunSemiAuto.generated.h"

// Semi automatic gun class
UCLASS()
class SHOOTERDEMO_API AGunSemiAuto : public AGunParent
{
	GENERATED_BODY()

	AGunSemiAuto();

	virtual void StartShooting() override;

	virtual void StartAlternateFire() override;

	virtual void StopAlternateFire() override;

	virtual void Tick(float DeltaTime) override;

	bool CanShoot = true;

	void ResetShooting();

};
