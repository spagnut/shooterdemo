// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterDemoProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "GunParent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"

AShooterDemoProjectile::AShooterDemoProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AShooterDemoProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 6000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 5 seconds by default
	InitialLifeSpan = 5.0f;
}

void AShooterDemoProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this)) {
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy != nullptr && !Enemy->IsDead)
		{
			// Apply damage and destroy projectile
			UGameplayStatics::ApplyDamage(Enemy, Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());

			Destroy();
		}
		else
		{
			// Only add impulse and destroy projectile if we hit a physics object
			if ((OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
			{
				OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

				Destroy();
			}
		}
	}
}