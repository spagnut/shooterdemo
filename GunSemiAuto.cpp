// Fill out your copyright notice in the Description page of Project Settings.


#include "GunSemiAuto.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterDemoCharacter.h"

AGunSemiAuto::AGunSemiAuto() {
	IsHitscan = false;
	GunName = FText::FromString("Launcher");
}

void AGunSemiAuto::ResetShooting() {
	CanShoot = true;
}

void AGunSemiAuto::StartShooting()
{
	if (CanShoot) {
		Super::StartShooting();
		Fire();
		CanShoot = false;

		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(
			UnusedHandle, this, &AGunSemiAuto::ResetShooting, FireRate, false);
	}
}

void AGunSemiAuto::StartAlternateFire()
{
	Super::StartAlternateFire();
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(this, 0.2F);
}

void AGunSemiAuto::StopAlternateFire()
{
	Super::StopAlternateFire();
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(GetParentActor(), 0.2F);
}

void AGunSemiAuto::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
