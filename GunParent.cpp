// Fill out your copyright notice in the Description page of Project Settings.

#include "GunParent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/DecalComponent.h"
#include "ShooterDemoCharacter.h"
#include "Camera/CameraComponent.h"
#include "ShooterDemoProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AudioComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Particles/ParticleSystem.h"

// Sets default values
AGunParent::AGunParent()
{
	PrimaryActorTick.bCanEverTick = true;

	DefRoot = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
	RootComponent = DefRoot;

	// Mesh
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetupAttachment(RootComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	// Gun
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(RootComponent);

	// Muzzle
	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);

	// Gun sights
	GunSight = CreateDefaultSubobject<UCameraComponent>(TEXT("GunSight"));
	GunSight->SetupAttachment(RootComponent);
	GunSight->SetRelativeLocation(FVector(44.66, 14.65, 153.68));
	GunSight->SetRelativeRotation(FRotator(0, 0, 16.3));

	// Muzzle flash
	MuzzleFlash = CreateDefaultSubobject<UParticleSystem>(TEXT("Muzzle Flash"));
}

// Called when the game starts or when spawned
void AGunParent::BeginPlay()
{
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
}

// Called every frame
void AGunParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGunParent::StartShooting()
{
	IsFiring = true;
}

void AGunParent::StopShooting()
{
	IsFiring = false;
}

void AGunParent::StartAlternateFire()
{
	IsAlternateFire = true;
	ACharacter* Char = Cast<ACharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (Char != nullptr)
		Char->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

void AGunParent::StopAlternateFire()
{
	IsAlternateFire = false;
	AShooterDemoCharacter* Char = Cast<AShooterDemoCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (Char != nullptr)
		Char->GetCharacterMovement()->MaxWalkSpeed = Char->WalkSpeed;
}

void AGunParent::Reload()
{
	if (IsReloading || (AmmoCurrent == ClipSize) || (AmmoRemaining == 0))
		return;

	IsReloading = true;

	if (IsAlternateFire)
		StopAlternateFire();

	// Play reload sound
	if (ReloadSound != nullptr)
	{
		ReloadSoundREF = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ReloadSound, GetActorLocation());
	}

	// Wait for ReloadTime so ammo doesnt reload instantly
	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(UnusedHandle, this,  &AGunParent::PerformReloadCalculations, ReloadTime, false);
}

void AGunParent::PerformReloadCalculations()
{
	if (IsValid(this) && IsReloading) {

		int BulletDifference = ClipSize - AmmoCurrent;

		if (AmmoRemaining >= BulletDifference) {
			AmmoCurrent = ClipSize;
			AmmoRemaining -= BulletDifference;
		}
		else
		{
			AmmoCurrent += AmmoRemaining;
			AmmoRemaining = 0;
		}

		IsReloading = false;
	}
}

// Stop playing reload sound if reload is interrupted
void AGunParent::StopReload()
{
	IsReloading = false;
	if (ReloadSoundREF != nullptr)
		ReloadSoundREF->ToggleActive();
}

void AGunParent::Fire()
{
	if (!IsValid(this)) return;

	if (IsReloading)
		StopReload();

	if (IsHitscan)
		FireBullet();
	else
		FireProjectile();
}

void AGunParent::FireBullet()
{
	if (AmmoCurrent > 0) {

		// Play fire sound
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetActorLocation());
		}

		// Report noise so AI enemies can hear it
		AActor* NoiseInstigator = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), FP_Gun->GetComponentLocation(), 1.0f, NoiseInstigator);

		USceneComponent* UsedCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetRootComponent();
		FVector TraceStart = UsedCamera->GetComponentLocation();
		FVector TraceEnd = TraceStart + UsedCamera->GetComponentRotation().Vector() * Range;
		FHitResult outHit;
		TArray<AActor*> ActorsToIgnore;

		// Perform line trace
		UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TraceEnd, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::Type::None, outHit, true);

		// Play fire animation
		if (FireAnimation != nullptr)
		{
			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			if (AnimInstance != nullptr)
				AnimInstance->Montage_Play(FireAnimation);
		}

		// Spawn muzzle flash
		if (MuzzleFlash != nullptr) {
			UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, FP_MuzzleLocation, NAME_None, (FVector(ForceInit)), FRotator::ZeroRotator, FVector(0.05 ,0.05,0.05), EAttachLocation::KeepRelativeOffset);
		}

		// Decrement ammo
		AmmoCurrent--;

		// Apply impulse to physics objects
		HitPhysicsObjects(outHit.bBlockingHit, outHit.GetComponent(), outHit.GetComponent(), outHit.Location, UsedCamera);

		if (outHit.bBlockingHit) {
			AEnemy* HitEnemy = Cast<AEnemy>(outHit.GetActor());
			if (HitEnemy != nullptr && !HitEnemy->IsDead)
			{				 
				// Apply damage
				if (outHit.BoneName == "Head")
				{
					// Head shot
					float DamageDone = UGameplayStatics::ApplyDamage(outHit.GetActor(), DamageHead, UGameplayStatics::GetPlayerController(GetWorld(), 0), nullptr, UDamageType::StaticClass());
				}
				else
				{
					// Body shot
					float DamageDone = UGameplayStatics::ApplyDamage(outHit.GetActor(), Damage, UGameplayStatics::GetPlayerController(GetWorld(), 0), nullptr, UDamageType::StaticClass());
				}
			}
			else
			{
				// Spawn decal
				UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletHoleDecal, FVector(10, 10, 10), outHit.Location, UKismetMathLibrary::MakeRotFromX(outHit.Normal), 50.0F);

				// Attach decal to hit object
				DecalComponent->AttachToComponent(outHit.GetComponent(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
			}
		}
	}
	else
	{
		// Play empty sound
		if (EmptySound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), EmptySound, GetActorLocation());
		}
	}
}

void AGunParent::HitPhysicsObjects(bool IsHittingSomething, UPrimitiveComponent* HitComponent, UPrimitiveComponent* ImpulseObject, FVector Location, USceneComponent* UsedCamera)
{
	if (IsHittingSomething) {
		if (HitComponent->IsSimulatingPhysics()) {
			FVector Impulse = ImpulseStrength * 1000 * UsedCamera->GetComponentRotation().Vector();

			ImpulseObject->AddImpulseAtLocation(Impulse, Location);
		}
	}
}

void AGunParent::FireProjectile()
{
	AShooterDemoCharacter* FPSChar = Cast<AShooterDemoCharacter>(GetParentActor());
	if (FPSChar != nullptr)
	{
		if (AmmoCurrent <= 0)
		{
			if (EmptySound != nullptr)
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), EmptySound, GetActorLocation());
		}
		else
		{
			// Play fire sound
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, FP_Gun->GetComponentLocation());
			}
			
			// Report noise so AI enemies can hear it
			AActor* NoiseInstigator = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
			UAISense_Hearing::ReportNoiseEvent(GetWorld(), FP_Gun->GetComponentLocation(), 1.0f, NoiseInstigator);

			// Determine projectile direction
			FRotator Direction;
			if (IsAlternateFire) 
				Direction = FP_MuzzleLocation->GetComponentRotation();			
			else
				Direction = FPSChar->GetFirstPersonCameraComponent()->GetComponentRotation();

			bool CanSpawnProjectile = false;

			// Try to fire a projectile
			if (ProjectileClass != nullptr)
			{
				const FRotator SpawnRotation = Direction;
				const FVector SpawnLocation = FP_MuzzleLocation->GetComponentLocation();

				// Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// Spawn the projectile at the muzzle
				AShooterDemoProjectile* Projectile = GetWorld()->SpawnActor<AShooterDemoProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

				if (Projectile != nullptr) {
					CanSpawnProjectile = true;
					Projectile->SetInstigator(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
					Projectile->Damage = Damage;
					Projectile->GetProjectileMovement()->InitialSpeed = BulletSpeed;
					Projectile->GetProjectileMovement()->MaxSpeed = BulletSpeed;
				}
			}

			// Fire animation
			if (FireAnimation != nullptr)
			{
				UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
				if (AnimInstance != nullptr)
				{
					AnimInstance->Montage_Play(FireAnimation, 1.f);
				}
			}

			// Spawn muzzle flash
			if (MuzzleFlash != nullptr && CanSpawnProjectile) {
				UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, FP_MuzzleLocation, NAME_None, (FVector(ForceInit)), FRotator::ZeroRotator, FVector(0.1, 0.1, 0.1), EAttachLocation::KeepRelativeOffset);
			}

			AmmoCurrent--;
		}
	}
}


