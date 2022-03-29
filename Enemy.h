// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

// Enemy class

UCLASS()
class SHOOTERDEMO_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	UPROPERTY(BlueprintReadWrite)
	bool IsDead = false;
	UPROPERTY(BlueprintReadWrite)
	bool IsAttacking = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthMax = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundWave* AggroSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundWave* AttackSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundWave* DeathSound;

	UPROPERTY(EditAnywhere, Category = "UI")
	class USphereComponent* HealthBarRadiusSphere;

	UPROPERTY(EditAnywhere, Category = "UI")
	class UWidgetComponent* HealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	class UBehaviorTree* TreeAsset;

protected:

	virtual void BeginPlay() override;

	void DeleteEnemy();

	class UAIHealthBar* HealthWidgetRef;

public:

	virtual void Tick(float DeltaTime) override;

	void ClearTarget();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, class AController* InstigatedBy, AActor* DamageCauser) override;

	void OnDeath();

	// Sets visibility of health bar widget
	UFUNCTION(BlueprintCallable)
	void OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult);

	// Unsets visibility of health bar widget
	UFUNCTION(BlueprintCallable)
	void OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex);
};
