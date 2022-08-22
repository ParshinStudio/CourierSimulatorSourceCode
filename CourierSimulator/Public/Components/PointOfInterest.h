// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PointOfInterest.generated.h"

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COURIERSIMULATOR_API UPointOfInterest : public UActorComponent
{
	GENERATED_BODY()

public:
	UPointOfInterest();
	UFUNCTION(BlueprintImplementableEvent)
	void TakeItem(bool QuestTaken, bool ItemTaken, int32 ID);

	virtual void BeginPlay() override;
};