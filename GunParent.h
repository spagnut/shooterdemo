// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.h"
#include "GunParent.generated.h"

class UCameraComponent;
class UAudioComponent;
class UParticleSystem;

// Parent class for all guns

UCLASS()
class SHOOTERDEMO_API AGunParent : public AActor
{
	GENERATED_BODY()
	
public:

	USceneComponent* DefRoot;

	// Pawn mesh: 1st person view (arms; seen only by self)
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	// Gun mesh: 1st person view (seen only by self)
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	// Location on gun mesh where projectiles should spawn.
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	// GunSight
	UPROPERTY(VisibleDefaultsOnly, Category = Camera)
	UCameraComponent* GunSight;

public:

	AGunParent();

	// Projectile class to spawn
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AShooterDemoProjectile> ProjectileClass;

	// AnimMontage to play each time we fire
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FText GunName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	EWeapons Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool IsFiring;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool IsAlternateFire;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool IsReloading;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	bool IsHitscan;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	int AmmoCurrent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	int AmmoRemaining;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	int ClipSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float FireRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float BulletSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float ReloadTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float Range;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float DamageHead;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float ImpulseStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundWave* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundWave* EmptySound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundWave* EquipSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundWave* ReloadSound;

	// Decal to draw when bullet hits something
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UMaterial* BulletHoleDecal;

	// What weapon to spawn when dropping weapon
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponToSpawn;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	virtual void StartShooting();
	
	virtual void StopShooting();
	
	virtual void StartAlternateFire();
	
	virtual void StopAlternateFire();

	UFUNCTION(BlueprintCallable)
	void Reload();

	void PerformReloadCalculations();

	UFUNCTION(BlueprintCallable)
	void StopReload();

	UFUNCTION(BlueprintCallable)
	void Fire();


private:

	UFUNCTION()
	void FireBullet();
	UFUNCTION()
	void FireProjectile();
	UFUNCTION()
	void HitPhysicsObjects(bool IsHittingSomething, UPrimitiveComponent* HitComponent, UPrimitiveComponent* ImpulseObject, FVector Location, USceneComponent* UsedCamera);

	UAudioComponent* ReloadSoundREF;

};
