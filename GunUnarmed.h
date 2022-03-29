// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GunParent.h"
#include "GunUnarmed.generated.h"

// Unarmed class

UCLASS()
class SHOOTERDEMO_API AGunUnarmed : public AGunParent
{
	GENERATED_BODY()

public:

	AGunUnarmed();

	virtual void StartShooting() override;

	virtual void StartAlternateFire() override;

	virtual void StopAlternateFire() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundWave* MissedSound;

private:
	bool CanPunch = true;

	void ResetPunching();

	void Punch();
};
