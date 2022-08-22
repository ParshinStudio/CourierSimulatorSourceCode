// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIControllerBaseCharacter.generated.h"

UCLASS()
class COURIERSIMULATOR_API AAIControllerBaseCharacter : public AAIController
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;
	void SetMoveToLocation(FVector Location) { LocationMoveTo = Location; };
	FVector LocationMoveTo;
};