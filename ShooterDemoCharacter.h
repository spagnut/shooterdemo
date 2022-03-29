// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapon.h"
#include "ShooterDemoCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;
class AController;

// Player character class

UCLASS(config=Game)
class AShooterDemoCharacter : public ACharacter
{
	GENERATED_BODY()

	// Arms and Gun
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UChildActorComponent* Arms;

	// First person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

public:

	AShooterDemoCharacter();

	UPROPERTY(BlueprintReadWrite)
	bool IsDead = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	float Health = 100;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	float HealthMax = 100;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	float WalkSpeed = 1100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MouseSensitivity = 1;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AGunParent> Fists;

protected:

	virtual void BeginPlay();

	void StartShooting();
	void StopShooting();

	// Alternate fire (viewing through gun sights)
	void StartAlternateFire();
	void StopAlternateFire();

	void Reload();

	void DropWeapon();

	void Pause();

	void StartCrouching();
	void StopCrouching();

	void MoveForward(float Val);
	void MoveRight(float Val);

	void Turn(float Val);
	void LookUp(float Val);

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Class Types")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	UUserWidget* HeadsUpDisplay;

	UPROPERTY(EditAnywhere, Category = "Class Types")
	TSubclassOf<UUserWidget> DeathWidgetClass;

	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	UUserWidget* DeathWidget;

	UPROPERTY(EditAnywhere, Category = "Class Types")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	UUserWidget* PauseMenu;

public:

	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	void EquipWeapon(AWeapon* PickedUpWeapon);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, class AController* InstigatedBy, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void OnDeath(AController* DeathInstigator);

	UFUNCTION(BlueprintCallable)
	UChildActorComponent* GetGun() { return Arms; }

private:

	void ChangeWeapon(AWeapon* PickedUpWeapon);

	void SpawnWeaponOnGround(TSubclassOf<AWeapon> WeaponToSpawn, EWeapons Type, int MaxAmmo, int CurrentAmmo);

};

