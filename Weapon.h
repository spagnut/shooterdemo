// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USphereComponent;
class USpotLightComponent;

UENUM()
enum class EWeapons : uint8 {
	Unarmed       UMETA(DisplayName = "Unarmed"),
	Kalash        UMETA(DisplayName = "Kalash"),
	Launcher      UMETA(DisplayName = "Launcher"),
};

// Weapons class to spawn on ground

UCLASS()
class SHOOTERDEMO_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:

	AWeapon();

	UPROPERTY(VisibleDefaultsOnly)
	USphereComponent* SphereCollision;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Gun;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USpotLightComponent* SpotLight;

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

	// Called when player overlaps weapon
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EWeaponTypes)
	EWeapons Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AmmoCurrent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AmmoRemaining;

	// Class of gun for player to equip
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AGunParent> WeaponToEquip;

};
