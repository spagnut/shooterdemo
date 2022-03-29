// Fill out your copyright notice in the Description page of Project Settings.


#include "GunFullyAuto.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterDemoCharacter.h"

AGunFullyAuto::AGunFullyAuto() {
	IsHitscan = true;
	GunName = FText::FromString("Kalash");
}

void AGunFullyAuto::ResetShooting() {
	CanShoot = true;
}

void AGunFullyAuto::StartShooting()
{
	if (CanShoot) {
		Super::StartShooting();
		Fire();
		CanShoot = false;

		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(
			UnusedHandle, this, &AGunFullyAuto::ResetShooting, FireRate, false);

		GetWorldTimerManager().SetTimer(AutoFireTimerHandle, this, &AGunFullyAuto::Fire, FireRate, true);

	}
}

void AGunFullyAuto::StopShooting()
{
	Super::StopShooting();
	GetWorldTimerManager().ClearTimer(AutoFireTimerHandle);
}

void AGunFullyAuto::StartAlternateFire()
{
	Super::StartAlternateFire();
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(this, 0.2F);
}

void AGunFullyAuto::StopAlternateFire()
{
	Super::StopAlternateFire();
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(GetParentActor(), 0.2F);
}

void AGunFullyAuto::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// When alternate fire is active, move camera accordingly
	if (IsAlternateFire)
	{
		AShooterDemoCharacter* Char = Cast<AShooterDemoCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

		if (Char != nullptr)
		{
			FRotator NewRotation = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation();

			Char->GetFirstPersonCameraComponent()->SetWorldRotation(NewRotation);
		}
	}
}
