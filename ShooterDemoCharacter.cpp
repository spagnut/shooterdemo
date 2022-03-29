// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterDemoCharacter.h"
#include "ShooterDemoProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enemy.h"
#include "GunParent.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "ShooterDemoGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

AShooterDemoCharacter::AShooterDemoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create arms and gun child component
	Arms = CreateDefaultSubobject<UChildActorComponent>(TEXT("Arms"));
	Arms->SetupAttachment(FirstPersonCameraComponent);
	Arms->SetRelativeLocation(FVector(0.0f, 13.0f, -151.0f));
	Arms->SetRelativeRotation(FRotator(0.0f, 3.0f, -16.0f));

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AShooterDemoCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(HUDWidgetClass)) {
		HeadsUpDisplay = CreateWidget(GetWorld(), HUDWidgetClass);
		if (HeadsUpDisplay != nullptr) {
			HeadsUpDisplay->AddToViewport();
		}
	}

	if (IsValid(DeathWidgetClass)) {
		DeathWidget = CreateWidget(GetWorld(), DeathWidgetClass);
	}

	if (IsValid(PauseMenuWidgetClass)) {
		PauseMenu = CreateWidget(GetWorld(), PauseMenuWidgetClass);
	}

	UWidgetBlueprintLibrary::SetInputMode_GameOnly(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}


// Input
void AShooterDemoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind crouch events
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterDemoCharacter::StartCrouching);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AShooterDemoCharacter::StopCrouching);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShooterDemoCharacter::StartShooting);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AShooterDemoCharacter::StopShooting);

	// Bind alternate fire event
	PlayerInputComponent->BindAction("AlternateFire", IE_Pressed, this, &AShooterDemoCharacter::StartAlternateFire);
	PlayerInputComponent->BindAction("AlternateFire", IE_Released, this, &AShooterDemoCharacter::StopAlternateFire);

	// Bind reload event
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AShooterDemoCharacter::Reload);
	PlayerInputComponent->BindAction("DropWeapon", IE_Pressed, this, &AShooterDemoCharacter::DropWeapon);

	// Bind pause event
	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AShooterDemoCharacter::Pause);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterDemoCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterDemoCharacter::MoveRight);

	// Bind turn events
	PlayerInputComponent->BindAxis("Turn", this, &AShooterDemoCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterDemoCharacter::LookUp);
}


void AShooterDemoCharacter::StartShooting()
{
	if (!IsDead)
		Cast<AGunParent>(Arms->GetChildActor())->StartShooting();
}

void AShooterDemoCharacter::StopShooting()
{
	if (!IsDead)
		Cast<AGunParent>(Arms->GetChildActor())->StopShooting();
}

void AShooterDemoCharacter::StartAlternateFire()
{
	if (!IsDead)
		Cast<AGunParent>(Arms->GetChildActor())->StartAlternateFire();
}

void AShooterDemoCharacter::StopAlternateFire()
{
	if (!IsDead)
		Cast<AGunParent>(Arms->GetChildActor())->StopAlternateFire();
}

void AShooterDemoCharacter::Reload()
{
	if (!IsDead)
		Cast<AGunParent>(Arms->GetChildActor())->Reload();
}

void AShooterDemoCharacter::Pause() {

	if (PauseMenu != nullptr) {

		UGameplayStatics::SetGamePaused(GetWorld(), true);

		PauseMenu->AddToViewport();

		APlayerController* PlayerCtrl = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		PlayerCtrl->SetShowMouseCursor(true);
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerCtrl);
	}
}

void AShooterDemoCharacter::MoveForward(float Value)
{
	if (!IsDead) {
		if (Value != 0.0f)
		{
			AddMovementInput(GetActorForwardVector(), Value);
		}
	}
}

void AShooterDemoCharacter::MoveRight(float Value)
{
	if (!IsDead) {
		if (Value != 0.0f)
		{
			AddMovementInput(GetActorRightVector(), Value);
		}
	}
}

void AShooterDemoCharacter::Turn(float Val)
{
	if (!IsDead) {
		if (Val != 0.0f)
		{
			AddControllerYawInput(MouseSensitivity * Val);
		}
	}
}

void AShooterDemoCharacter::LookUp(float Val)
{
	if (!IsDead) {
		if (Val != 0.0f)
		{
			AddControllerPitchInput(MouseSensitivity * Val);
		}
	}
}

void AShooterDemoCharacter::StartCrouching() {
	if (!IsDead)
		Crouch();
}

void AShooterDemoCharacter::StopCrouching() {
	if (!IsDead)
		UnCrouch();
}

void AShooterDemoCharacter::DropWeapon()
{
	AGunParent* Gun = Cast<AGunParent>(Arms->GetChildActor());

	if (Gun->Type != EWeapons::Unarmed) {
		SpawnWeaponOnGround(Gun->WeaponToSpawn, Gun->Type, Gun->AmmoRemaining, Gun->AmmoCurrent);

		ChangeWeapon(nullptr);
	}
}

void AShooterDemoCharacter::EquipWeapon(AWeapon* PickedUpWeapon)
{
	AGunParent* Gun = Cast<AGunParent>(Arms->GetChildActor());

	if (Gun->Type != EWeapons::Unarmed)
		SpawnWeaponOnGround(Gun->WeaponToSpawn, Gun->Type, Gun->AmmoRemaining, Gun->AmmoCurrent);

	ChangeWeapon(PickedUpWeapon);

}

void AShooterDemoCharacter::ChangeWeapon(AWeapon* PickedUpWeapon)
{
	AGunParent* Gun = Cast<AGunParent>(Arms->GetChildActor());
	if (Gun->IsAlternateFire)
		Gun->StopAlternateFire();
	if (Gun->IsReloading)
		Gun->StopReload();
	if (Gun->IsFiring)
		Gun->StopShooting();

	TSubclassOf<AGunParent> NextWeapon = Fists;
	int CurrAmmo = 0;
	int RemainAmmo = 0;

	if (PickedUpWeapon) {
		CurrAmmo = PickedUpWeapon->AmmoCurrent;
		RemainAmmo = PickedUpWeapon->AmmoRemaining;
		NextWeapon = PickedUpWeapon->WeaponToEquip;

		PickedUpWeapon->Destroy();
	}

	// Check if nextweapon is valid
	if (NextWeapon != nullptr) {
		Arms->SetChildActorClass(NextWeapon);

		Gun = Cast<AGunParent>(Arms->GetChildActor());
		Gun->AmmoCurrent = CurrAmmo;
		Gun->AmmoRemaining = RemainAmmo;
	}

	// Play equip sound
	if (Gun->EquipSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Gun->EquipSound, Gun->GetActorLocation());
	}

	// Refresh HUD widget
	HeadsUpDisplay->RemoveFromViewport();
	HeadsUpDisplay = CreateWidget(GetWorld(), HUDWidgetClass);
	HeadsUpDisplay->AddToViewport();

}

void AShooterDemoCharacter::SpawnWeaponOnGround(TSubclassOf<AWeapon> WeaponToSpawn, EWeapons Type, int RemainingAmmo, int CurrentAmmo)
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	FVector PlayerLocation = Player->GetActorLocation();

	float Distance = UKismetMathLibrary::Vector_Distance2D(PlayerLocation, PlayerLocation + Player->GetControlRotation().Vector() * 300);

	FVector2D SpawnLocation;

	if (Distance > 100) {
		SpawnLocation = FVector2D(PlayerLocation + Player->GetControlRotation().Vector() * 300);
	}
	else {
		SpawnLocation.X = PlayerLocation.X + 300;
		SpawnLocation.Y = PlayerLocation.Y + 300;
	}

	if (WeaponToSpawn) {
		FVector Location(SpawnLocation.X, SpawnLocation.Y, PlayerLocation.Z - 30);
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;

		AWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponToSpawn, Location, Rotation, SpawnInfo);

		SpawnedWeapon->AmmoCurrent = CurrentAmmo;
		SpawnedWeapon->AmmoRemaining = RemainingAmmo;
	}

}

void AShooterDemoCharacter::OnDeath(AController* DeathInstigator)
{
	IsDead = true;

	AShooterDemoGameMode* GameMode = Cast<AShooterDemoGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	for (AEnemy* Enemy : GameMode->AllEnemies) {
		if (IsValid(Enemy))
			Enemy->ClearTarget();
	}

	// Stop shooting automatic weapon
	AGunParent* CurrentGun = Cast<AGunParent>(GetGun()->GetChildActor());
	if (CurrentGun->Type == EWeapons::Kalash) {
		CurrentGun->StopShooting();
	}

	if (DeathWidget != nullptr) {
		DeathWidget->AddToViewport();
	}

	APlayerController* PlayerCtrl = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerCtrl, DeathWidget);
	PlayerCtrl->SetShowMouseCursor(true);

	GameMode->GameOver(true);
}

float AShooterDemoCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsDead) {

		Health = Health - DamageAmount;
		if (Health < 0)
			Health = 0;
		if (Health > HealthMax)
			Health = HealthMax;

		if (Health == 0)
			OnDeath(InstigatedBy);

		return DamageAmount;
	}

	return -1;
}
