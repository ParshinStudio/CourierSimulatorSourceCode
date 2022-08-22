// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/BaseCharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Character/BaseCharacter.h"

float UBaseCharacterMovementComponent::GetMaxSpeed() const
{
	const float MaxSpeed = Super::GetMaxSpeed();
	const ABaseCharacter* Player = Cast<ABaseCharacter>(GetPawnOwner());
	return Player && Player->IsRunning() ? MaxSpeed * RunModifier : MaxSpeed;
}