// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "AICEnemy.h"
#include "BrainComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "AIHealthBar.h"
#include "ShooterDemoCharacter.h"
#include "ShooterDemoGameMode.h"

// Sets default values
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add(FName("Enemy"));

	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(RootComponent);
	HealthBar->SetRelativeLocation(FVector(0, 0, 117.f));

	HealthBarRadiusSphere = CreateDefaultSubobject<USphereComponent>(TEXT("HealthBarRadiusSphere"));
	HealthBarRadiusSphere->SetupAttachment(RootComponent);

	FVector Scale = FVector(1.0f);
	HealthBarRadiusSphere->SetWorldScale3D(Scale * 50.f);

	GetCapsuleComponent()->bApplyImpulseOnDamage = false;
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);

	GetMesh()->SetNotifyRigidBodyCollision(true);
	GetMesh()->SetGenerateOverlapEvents(true);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (HealthBar != nullptr) {

		HealthBar->SetVisibility(false);

		HealthWidgetRef = Cast<UAIHealthBar>(HealthBar->GetUserWidgetObject());
		if (HealthWidgetRef != nullptr) {
			HealthWidgetRef->HealthMax = HealthMax;
			HealthWidgetRef->Health = Health;
		}
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemy::OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool FromSweet, const FHitResult& SweepResult) {
	if (Cast<AShooterDemoCharacter>(OtherActor) != nullptr) {
		if (HealthBar != nullptr)
			HealthBar->SetVisibility(true);
	}
}

void AEnemy::OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex) {
	if (Cast<AShooterDemoCharacter>(OtherActor) != nullptr) {
		if (HealthBar != nullptr)
			HealthBar->SetVisibility(false);
	}
}

void AEnemy::ClearTarget()
{
	Cast<AAICEnemy>(GetController())->ClearTarget();
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsDead) {

		Health = Health - DamageAmount;
		if (Health < 0)
			Health = 0;
		if (Health > HealthMax)
			Health = HealthMax;

		// Set health widget visibility when attacked by player
		if (HealthWidgetRef != nullptr)
			HealthWidgetRef->Health = Health;
		if (HealthBar != nullptr)
			HealthBar->SetVisibility(true);

		if (Health == 0)
			OnDeath();
		else {
			Cast<AAICEnemy>(GetController())->SetTarget(InstigatedBy);
		}

		return DamageAmount;
	}

	return -1;
}

void AEnemy::OnDeath() {

	IsDead = true;

	UBrainComponent* Brain = Cast<AAIController>(GetController())->GetBrainComponent();
	Brain->StopLogic("Death");

	if (DeathSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
	}

	AShooterDemoGameMode* GameModeRef = Cast<AShooterDemoGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	// Check if the last enemy is eliminated
	if (--GameModeRef->EnemiesLeft == 0)
		GameModeRef->GameOver(false);

	// Destroy this actor after 5 seconds
	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(
		UnusedHandle, this, &AEnemy::DeleteEnemy, 5.0f, false);

}

void AEnemy::DeleteEnemy() {
	Destroy();
}


