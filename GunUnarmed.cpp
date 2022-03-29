// Fill out your copyright notice in the Description page of Project Settings.


#include "GunUnarmed.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "ShooterDemoGameMode.h"
#include "ShooterDemoCharacter.h"
#include "Camera/CameraComponent.h"
#include "Enemy.h"

AGunUnarmed::AGunUnarmed() {
	GunName = FText::FromString("Fists");
}

void AGunUnarmed::StartShooting()
{
	if (CanPunch) {

		Punch();

		CanPunch = false;

		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(
		UnusedHandle, this, &AGunUnarmed::ResetPunching, FireRate, false);
	}
}

void AGunUnarmed::StartAlternateFire()
{
	// Does nothing for unarmed
}

void AGunUnarmed::StopAlternateFire()
{
	// Does nothing for unarmed
}

void AGunUnarmed::ResetPunching()
{
	CanPunch = true;
}

void AGunUnarmed::Punch()
{
	// Play animation
	if (FireAnimation != nullptr)
	{
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
			AnimInstance->Montage_Play(FireAnimation);
	}

	AShooterDemoGameMode* GameMode = Cast<AShooterDemoGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	AActor* EnemyHit = nullptr;

	// Go through the array of all spawned enemies
	for (AEnemy* Enemy : GameMode->AllEnemies) {

		// Enemy has already been destroyed
		if (!IsValid(Enemy))
			continue;

		// Enemy is dead
		if (Enemy->IsDead)
			continue;

		
		AShooterDemoCharacter* PlayerChar = Cast<AShooterDemoCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

		FVector EnemyLocation = Enemy->GetActorLocation();
		FVector PlayerLocation = PlayerChar->GetActorLocation();

		// Distance between player and enemy
		float Distance = UKismetMathLibrary::Vector_Distance(EnemyLocation, PlayerLocation);

		bool IsFacingEnemy = false;

		FVector PlayerDirection = PlayerChar->GetFirstPersonCameraComponent()->GetForwardVector();

		FRotator RotationToLookAtEnemy = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, EnemyLocation);
		FVector IdealPlayerDirection = RotationToLookAtEnemy.Vector();

		// If the player is looking directly at enemy, dot product will be close to 1
		float Prod = FVector::DotProduct(PlayerDirection, IdealPlayerDirection);
		if (Prod > 0.75) {
			IsFacingEnemy = true;
		}

		// Enemy is close enough and player is facing enemy	
		if (Distance <= 200 && IsFacingEnemy) {
			EnemyHit = Enemy;
			break;
		}
	}

	if (EnemyHit != nullptr)
	{
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetActorLocation());
		}

		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		UGameplayStatics::ApplyDamage(EnemyHit, Damage, PlayerController, nullptr, NULL);

	}
	else
	{
		if (MissedSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), MissedSound, GetActorLocation());
		}
	}
}

