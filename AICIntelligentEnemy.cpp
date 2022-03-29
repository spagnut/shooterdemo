// Fill out your copyright notice in the Description page of Project Settings.


#include "AICIntelligentEnemy.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ShooterDemoCharacter.h"

AAICIntelligentEnemy::AAICIntelligentEnemy()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));

	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AAICIntelligentEnemy::OnPawnDetected);
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->ConfigureSense(*HearingConfig);

	DefaultWalkSpeed = 450.f;
	CombatWalkSpeed = 900.f;
}

void AAICIntelligentEnemy::OnPawnDetected(const TArray<AActor*>& DetectedPawns)
{
	if (!IsValid(BlackboardUsed->GetValueAsObject(FName("Enemy"))))
	{
		AShooterDemoCharacter* PlayerChar = nullptr;
		bool PlayerFound = false;

		for (AActor* PotentialPlayer : DetectedPawns) {

			PlayerChar = Cast<AShooterDemoCharacter>(PotentialPlayer);

			if (PlayerChar != nullptr)
				if (!PlayerChar->IsDead) {
					PlayerFound = true;
					break;
				}
		}

		if (PlayerFound) {

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
}
