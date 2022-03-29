// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/SpotLightComponent.h"
#include "ShooterDemoCharacter.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));

	RootComponent = SphereCollision;

	Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	Gun->SetupAttachment(RootComponent);

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(RootComponent);
	SpotLight->SetRelativeRotation(FRotator(-90, 0, 0));
	SpotLight->SetRelativeLocation(FVector(0, 0, 70));

	// Add event overlap begin
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnOverlapBegin);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Weapon is constantly rotating to be more visible
	RootComponent->AddLocalRotation(FRotator(0, 1, 0));
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Player equips weapon on overlap
	AShooterDemoCharacter* Player = Cast<AShooterDemoCharacter>(OtherActor);
	if (Player != nullptr)
		Player->EquipWeapon(this);
}

