// Fill out your copyright notice in the Description page of Project Settings.


#include "AICEnemy.h"
#include "Enemy.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AAICEnemy::AAICEnemy() {

	BlackboardUsed = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));

	BehaviorTreeUsed = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTree Component"));

	DefaultWalkSpeed = 200.f;
	CombatWalkSpeed = 750.f;
}

void AAICEnemy::OnPossess(APawn* InPawn) {

	Super::OnPossess(InPawn);

	AEnemy* ControlledEnemy = Cast<AEnemy>(InPawn);

	if (ControlledEnemy != nullptr && ControlledEnemy->TreeAsset != nullptr) {

		// Initalize blackboard
		BlackboardUsed->InitializeBlackboard(*ControlledEnemy->TreeAsset->BlackboardAsset);

		// Set Blackboard key IDs
		EnemyKeyId = BlackboardUsed->GetKeyID("Enemy");
		TargetLocationKeyId = BlackboardUsed->GetKeyID("TargetLocation");

		// Start the behavior tree
		BehaviorTreeUsed->StartTree(*ControlledEnemy->TreeAsset);

		ControlledEnemy->GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	}
}


void AAICEnemy::SetTarget(AController* DamageInstigator)
{
	// Check if Enemy isn't already set
	if (!IsValid(BlackboardUsed->GetValueAsObject(FName("Enemy"))))
	{
		APawn* PlayerChar = Cast<APawn>(DamageInstigator->GetPawn());

		BlackboardUsed->SetValueAsObject(FName("Enemy"), PlayerChar);

		// Play aggro sound
		AEnemy* ControlledEnemy = Cast<AEnemy>(GetPawn());
		USoundBase* AggroSound = ControlledEnemy->AggroSound;
		if (AggroSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), AggroSound, GetPawn()->GetActorLocation());
		}

		// Set combat walk speed
		ControlledEnemy->GetCharacterMovement()->MaxWalkSpeed = CombatWalkSpeed;
	}
}

void AAICEnemy::ClearTarget() {

	BlackboardUsed->SetValueAsObject(FName("Enemy"), nullptr);

	// Set default walk speed
	AEnemy* ControlledEnemy = Cast<AEnemy>(GetPawn());
	ControlledEnemy->GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
}

void AAICEnemy::Attack()
{
	AActor* Target = Cast<AActor>(BlackboardUsed->GetValueAsObject(FName("Enemy")));

	AEnemy* ControlledEnemy = Cast<AEnemy>(GetPawn());

	if (Target != nullptr) {

		UGameplayStatics::ApplyDamage(Target, ControlledEnemy->Damage, this, nullptr, UDamageType::StaticClass());

		// Play attack sound
		USoundBase* AttackSound = Cast<AEnemy>(GetPawn())->AttackSound;
		if (AttackSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSound, GetPawn()->GetActorLocation());
		}
	}

	ControlledEnemy->IsAttacking = false;
}