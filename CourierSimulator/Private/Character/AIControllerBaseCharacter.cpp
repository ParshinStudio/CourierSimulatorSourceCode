// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/AIControllerBaseCharacter.h"
#include "Character/BaseCharacter.h"

void AAIControllerBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (FVector::Dist(FVector(GetPawn()->GetActorLocation().X, GetPawn()->GetActorLocation().Y, 0.0f), FVector(LocationMoveTo.X, LocationMoveTo.Y, 0.0f)) < 3.0f)
	{
		auto PlayerChar = Cast<ABaseCharacter>(GetPawn());
		if (!PlayerChar) return;
		PrimaryActorTick.SetTickFunctionEnable(false);
		PlayerChar->UnPossessAIController();

	}
}