// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VehicleAnimInstance.h"
#include "BaseCarAnimInstance.generated.h"

UCLASS()
class COURIERSIMULATOR_API UBaseCarAnimInstance : public UVehicleAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float SWRot;
};
